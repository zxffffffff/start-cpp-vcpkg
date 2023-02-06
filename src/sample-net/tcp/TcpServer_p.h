#pragma once
#include "TcpCommon.h"
#include <uv.h>

class TcpServerPrivate
{
    // param
public:
    const char* ip;
    int port;
    HandleRun handleRun;
    HandleClose handleClose;
    HandleNewConn handleNewConn;
    HandleCloseConn handleCloseConn;
    HandleConnOnRead handleConnOnRead;

    // tcp
public:
    std::atomic_bool running{ false };
    std::shared_ptr<uv_async_t> async_close = std::make_shared<uv_async_t>();
    std::shared_ptr<uv_async_t> async_write = std::make_shared<uv_async_t>();
    std::unordered_map<SocketPtr, std::vector<char>> write_map;
    std::mutex write_mutex;
private:
    std::shared_ptr<uv_loop_t> loop = std::make_shared<uv_loop_t>();
    std::shared_ptr<uv_tcp_t> server = std::make_shared<uv_tcp_t>();
    sockaddr_in addr{ 0 };
    int listen_backlog = 128;
    std::list<std::shared_ptr<uv_tcp_t>> clients;
    std::vector<char> read_buf;

public:
    TcpServerPrivate(const char* ip, int port)
        : ip(ip), port(port)
    {
    }

    ~TcpServerPrivate()
    {
    }

    static void onRun(void* arg)
    {
        auto priv = (TcpServerPrivate*)arg;
        uv_loop_init(priv->loop.get());

        do {
            uv_tcp_init(priv->loop.get(), priv->server.get());
            priv->server->data = priv;

            uv_ip4_addr(priv->ip, priv->port, &priv->addr);

            uv_tcp_bind(priv->server.get(), (const struct sockaddr*)&priv->addr, 0);

            int r = uv_listen((uv_stream_t*)priv->server.get(), priv->listen_backlog, onConnection);
            if (r) {
                LOG(ERROR) << "Listen error " << uv_strerror(r);
                break;
            }
            priv->running = true;

            priv->async_close->data = priv;
            priv->async_write->data = priv;
            uv_async_init(priv->loop.get(), priv->async_close.get(), TcpServerPrivate::onAsyncClose);
            uv_async_init(priv->loop.get(), priv->async_write.get(), TcpServerPrivate::onAsyncWrite);

            LOG(INFO) << "server run " << priv->ip << " : " << priv->port;
            if (priv->handleRun)
                priv->handleRun();
            uv_run(priv->loop.get(), UV_RUN_DEFAULT);
            LOG(INFO) << "server stop " << priv->ip << " : " << priv->port;
        } while (0);

        uv_loop_close(priv->loop.get());
    }

    static void onAsyncClose(uv_async_t* handle)
    {
        LOG(INFO) << "onAsyncClose";
        auto priv = (TcpServerPrivate*)handle->data;
        uv_close((uv_handle_t*)priv->server.get(), TcpServerPrivate::onClose);
    }

    static void onClose(uv_handle_t* server)
    {
        LOG(INFO) << "onClose";
        auto priv = (TcpServerPrivate*)server->data;
        priv->running = false;
        priv->clients.clear();

        if (priv->handleClose)
            priv->handleClose();
    }

    static void onConnection(uv_stream_t* server, int status)
    {
        if (status < 0) {
            LOG(ERROR) << "onConnection error " << uv_strerror(status);
            return;
        }

        LOG(INFO) << "onConnection";
        auto priv = (TcpServerPrivate*)server->data;

        priv->clients.push_back(std::make_shared<uv_tcp_t>());
        auto client = priv->clients.back();
        uv_tcp_init(priv->loop.get(), client.get());
        client->data = priv;

        if (uv_accept(server, (uv_stream_t*)client.get()) == 0) {
            if (priv->handleNewConn)
                priv->handleNewConn(client.get());
            
            uv_read_start((uv_stream_t*)client.get(), onReadAllocCbk, onReadCbk);
        }
        else {
            uv_close((uv_handle_t*)client.get(), onCloseConnection);
        }
    }

    static void onCloseConnection(uv_handle_t* client)
    {
        LOG(INFO) << "onCloseConnection";
        auto priv = (TcpServerPrivate*)client->data;
        for (auto ite = priv->clients.begin(); ite != priv->clients.end(); ++ite) {
            if (ite->get() == (uv_tcp_t*)client) {
                if (priv->handleCloseConn)
                    priv->handleCloseConn(client);

                priv->clients.erase(ite);
                break;
            }
        }
    }

    static void onReadAllocCbk(uv_handle_t* client, size_t suggested_size, uv_buf_t* buf)
    {
        auto priv = (TcpServerPrivate*)client->data;
        priv->read_buf.resize(suggested_size);
        *buf = uv_buf_init(priv->read_buf.data(), suggested_size);
    }

    static void onReadCbk(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf)
    {
        auto priv = (TcpServerPrivate*)client->data;
        if (nread > 0) {
            LOG(INFO) << "onReadCbk " << std::string(buf->base, nread);

            if (priv->handleConnOnRead)
                priv->handleConnOnRead(client, buf->base, nread);
        }
        if (nread < 0 || nread == UV_EOF) {
            LOG(INFO) << "onReadCbk close " << nread;
            uv_close((uv_handle_t*)client, onCloseConnection);
        }
    }

    static void onAsyncWrite(uv_async_t* handle)
    {
        LOG(INFO) << "onAsyncWrite";
        auto priv = (TcpServerPrivate*)handle->data;

        std::unordered_map<SocketPtr, std::vector<char>> write_map;
        {
            std::lock_guard<std::mutex> guard(priv->write_mutex);
            std::swap(write_map, priv->write_map);
        }

        for (auto ite = write_map.begin(); ite != write_map.end(); ++ite) {
            auto& socket = ite->first;
            auto& write_buf = ite->second;
            
            bool flag = false;
            for (auto ite = priv->clients.begin(); ite != priv->clients.end(); ++ite) {
                if (ite->get() == socket) {
                    flag = true;
                    break;
                }
            }
            if (!flag)
                continue;

            LOG(INFO) << "Write " << std::string(write_buf.data(), write_buf.size());
            auto req = new uv_write_t;
            auto bufs = uv_buf_init(write_buf.data(), write_buf.size());
            int r = ::uv_write(req, (uv_stream_t*)socket, &bufs, 1, [](uv_write_t* req, int status) { delete req; });
            if (r) {
                LOG(INFO) << "Write error " << r;
                delete req;
            }
        }
    }
};
