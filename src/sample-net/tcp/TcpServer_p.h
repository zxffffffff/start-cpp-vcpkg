#pragma once
#include "TcpCommon.h"
#include <uv.h>

class TcpServerPrivate
{
    friend class TcpServer;
private:
    // param
    const char* ip;
    int port;
    HandleRun handleRun;
    HandleClose handleClose;
    HandleNewConn handleNewConn;
    HandleCloseConn handleCloseConn;
    HandleConnOnRead handleConnOnRead;

    // tcp
    bool running{ false };
    std::shared_ptr<uv_loop_t> loop = std::make_shared<uv_loop_t>();
    std::shared_ptr<uv_tcp_t> server = std::make_shared<uv_tcp_t>();
    sockaddr_in addr{ 0 };
    int listen_backlog = 128;
    std::list<std::shared_ptr<uv_tcp_t>> clients;

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
                fprintf(stderr, "Listen error %s\n", uv_strerror(r));
                break;
            }
            priv->running = true;

            uv_run(priv->loop.get(), UV_RUN_DEFAULT);

            uv_close((uv_handle_t*)priv->server.get(), TcpServerPrivate::onClose);
        } while (0);

        uv_loop_close(priv->loop.get());
    }

    static void onClose(uv_handle_t* server)
    {
        auto priv = (TcpServerPrivate*)server->data;
        priv->running = false;
        priv->clients.clear();
    }

    static void onConnection(uv_stream_t* server, int status)
    {
        if (status < 0) {
            fprintf(stderr, "onConnection error %s\n", uv_strerror(status));
            return;
        }

        auto priv = (TcpServerPrivate*)server->data;

        priv->clients.push_back(std::make_shared<uv_tcp_t>());
        auto client = priv->clients.back();
        uv_tcp_init(priv->loop.get(), client.get());
        client->data = priv;

        if (uv_accept(server, (uv_stream_t*)client.get()) == 0) {
            if (priv->handleNewConn) {
                priv->handleNewConn((size_t)client.get());
            }
            uv_read_start((uv_stream_t*)client.get(), onReadAllocCbk, onReadCbk);
        }
        else {
            uv_close((uv_handle_t*)client.get(), onCloseConnection);
        }
    }

    static void onCloseConnection(uv_handle_t* client)
    {
        auto priv = (TcpServerPrivate*)client->data;
        for (auto ite = priv->clients.begin(); ite != priv->clients.end(); ++ite) {
            if (ite->get() == (uv_tcp_t*)client) {
                priv->clients.erase(ite);
                break;
            }
        }
    }

    static void onReadAllocCbk(uv_handle_t* client, size_t suggested_size, uv_buf_t* buf)
    {
        *buf = uv_buf_init(new char[suggested_size], suggested_size);
    }

    static void onReadCbk(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf)
    {
        auto priv = (TcpServerPrivate*)client->data;
        if (nread > 0) {
            if (priv->handleConnOnRead) {
                priv->handleConnOnRead((size_t)client, buf->base, nread);
            }
        }
        if (nread < 0 || nread == UV_EOF) {
            uv_close((uv_handle_t*)client, onCloseConnection);
        }
    }
};
