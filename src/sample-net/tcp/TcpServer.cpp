#include "TcpServer.h"
#include <uv.h>

class TcpServerPrivate
{
public:
    // param
    const char* ip;
    int port;
    std::list<std::shared_ptr<ITcpServerHandler>> handlers;

    // tcp
    uv_loop_t loop{ 0 };
    uv_tcp_t server{ 0 };
    sockaddr_in addr{ 0 };
    int listen_backlog = 128;
    std::list<uv_tcp_t> clients;

public:
    TcpServerPrivate(const char* ip, int port)
        : ip(ip), port(port)
    {
    }

    static void onRunServer(void* arg)
    {
        auto priv = (TcpServerPrivate*)arg;
        uv_loop_init(&priv->loop);

        do {
            uv_tcp_init(&priv->loop, &priv->server);
            priv->server.data = priv;

            uv_ip4_addr(priv->ip, priv->port, &priv->addr);

            uv_tcp_bind(&priv->server, (const struct sockaddr*)&priv->addr, 0);

            int r = uv_listen((uv_stream_t*)&priv->server, priv->listen_backlog, onNewConnection);
            if (r) {
                fprintf(stderr, "Listen error %s\n", uv_strerror(r));
                break;
            }

            uv_run(&priv->loop, UV_RUN_DEFAULT);
        } while (0);

        uv_loop_close(&priv->loop);
    }

    static void onNewConnection(uv_stream_t* server, int status)
    {
        if (status < 0) {
            fprintf(stderr, "onNewConnection error %s\n", uv_strerror(status));
            return;
        }

        auto priv = (TcpServerPrivate*)server->data;

        uv_tcp_t client;
        uv_tcp_init(&priv->loop, &client);
        client.data = priv;

        if (uv_accept(server, (uv_stream_t*)&client) == 0) {
            uv_read_start((uv_stream_t*)&client, onReadAllocCbk, onReadCbk);
        }
        else {
            uv_close((uv_handle_t*)&client, NULL);
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
            for (auto& handler : priv->handlers) {
                handler->OnRead(buf->base, nread);
            }
        }
        if (nread < 0 || nread == UV_EOF) {
            uv_close((uv_handle_t*)&client, NULL);
        }
    }
};

TcpServer::TcpServer(const char* ip, int port)
    : priv(std::make_unique<TcpServerPrivate>(ip, port))
{
}

TcpServer::~TcpServer()
{
}

void TcpServer::AddHandler(std::shared_ptr<ITcpServerHandler> Handler)
{
    priv->handlers.push_back(Handler);
}

void TcpServer::Run()
{
    uv_thread_t thread_id;
    uv_thread_create(&thread_id, TcpServerPrivate::onRunServer, priv.get());
}

void TcpServer::Close()
{
    uv_close((uv_handle_t*)&priv->server, NULL);
}
