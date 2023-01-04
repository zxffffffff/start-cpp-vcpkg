#pragma once
#include "TcpCommon.h"
#include <uv.h>

class TcpClientPrivate
{
    friend class TcpClient;
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
    sockaddr_in dest{ 0 };
    std::shared_ptr<uv_connect_t> connect = std::make_shared<uv_connect_t>();

public:
    TcpClientPrivate(const char* ip, int port)
        : ip(ip), port(port)
    {
    }

    ~TcpClientPrivate()
    {
    }

    static void onRun(void* arg)
    {
        auto priv = (TcpClientPrivate*)arg;
        uv_loop_init(priv->loop.get());

        do {
            uv_tcp_init(priv->loop.get(), priv->socket.get());
            priv->socket.data = priv;

            uv_ip4_addr(priv->ip, priv->port, &priv->dest);

            int r = uv_tcp_connect(priv->connect.get(), priv->socket.get(), (const struct sockaddr*)&priv->dest, onConnection);
            if (r) {
                fprintf(stderr, "Connect error %s\n", uv_strerror(r));
                break;
            }
            priv->connect.data = priv;
            priv->running = true;

            uv_run(priv->loop.get(), UV_RUN_DEFAULT);

            uv_close((uv_handle_t*)priv->socket.get(), TcpClientPrivate::onClose);
        } while (0);

        uv_loop_close(priv->loop.get());
    }

    static void onClose(uv_handle_t* socket)
    {
        auto priv = (TcpClientPrivate*)socket->data;
        priv->running = false;
    }

    static void onConnection(uv_connect_t* connect, int status)
    {
        if (status < 0) {
            fprintf(stderr, "onConnection error %s\n", uv_strerror(status));
            return;
        }

        auto priv = (TcpClientPrivate*)connect->data;

        uv_read_start((uv_stream_t*)priv->socket.get(), onReadAllocCbk, onReadCbk);
    }

    static void onCloseConnection(uv_handle_t* connect)
    {
        auto priv = (TcpClientPrivate*)connect->data;
    }

    static void onReadAllocCbk(uv_handle_t* socket, size_t suggested_size, uv_buf_t* buf)
    {
        *buf = uv_buf_init(new char[suggested_size], suggested_size);
    }

    static void onReadCbk(uv_stream_t* socket, ssize_t nread, const uv_buf_t* buf)
    {
        auto priv = (TcpClientPrivate*)socket->data;
        if (nread > 0) {
            if (priv->handleConnOnRead) {
                priv->handleConnOnRead((size_t)socket, buf->base, nread);
            }
        }
        if (nread < 0 || nread == UV_EOF) {
            uv_close((uv_handle_t*)socket.get(), onCloseConnection);
        }
    }
};
