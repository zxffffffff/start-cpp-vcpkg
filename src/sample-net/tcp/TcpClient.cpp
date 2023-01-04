#include "TcpClient.h"
#include <uv.h>

class TcpClientPrivate
{
public:
    // param
    const char* ip;
    int port;
    std::list<std::shared_ptr<ITcpClientHandler>> handlers;

    // tcp
    uv_loop_t loop{ 0 };
    uv_tcp_t socket{ 0 };
    sockaddr_in dest{ 0 };
    uv_connect_t connect{ 0 };

public:
    TcpClientPrivate(const char* ip, int port)
        : ip(ip), port(port)
    {
    }

    static void onRunClient(void* arg)
    {
        auto priv = (TcpClientPrivate*)arg;
        uv_loop_init(&priv->loop);

        do {
            uv_tcp_init(&priv->loop, &priv->socket);
            priv->socket.data = priv;

            uv_ip4_addr(priv->ip, priv->port, &priv->dest);

            int r = uv_tcp_connect(&priv->connect, &priv->socket, (const struct sockaddr*)&priv->dest, [](uv_connect_t* req, int status) {});
            if (r) {
                fprintf(stderr, "Connect error %s\n", uv_strerror(r));
                break;
            }
            priv->connect.data = priv;

            uv_run(&priv->loop, UV_RUN_DEFAULT);
        } while (0);

        uv_loop_close(&priv->loop);
    }

    static void onConnection(uv_connect_t* req, int status)
    {
        if (status < 0) {
            fprintf(stderr, "onConnection error %s\n", uv_strerror(status));
            return;
        }

        auto priv = (TcpClientPrivate*)req->data;

        uv_read_start((uv_stream_t*)&priv->socket, onReadAllocCbk, onReadCbk);
    }

    static void onReadAllocCbk(uv_handle_t* socket, size_t suggested_size, uv_buf_t* buf)
    {
        *buf = uv_buf_init(new char[suggested_size], suggested_size);
    }

    static void onReadCbk(uv_stream_t* socket, ssize_t nread, const uv_buf_t* buf)
    {
        auto priv = (TcpClientPrivate*)socket->data;
        if (nread > 0) {
            for (auto& handler : priv->handlers) {
                handler->OnRead(buf->base, nread);
            }
        }
        if (nread < 0 || nread == UV_EOF) {
            uv_close((uv_handle_t*)&socket, NULL);
        }
    }
};

TcpClient::TcpClient(const char* ip, int port)
    : priv(std::make_unique<TcpClientPrivate>(ip, port))
{
}

TcpClient::~TcpClient()
{
}

void TcpClient::AddHandler(std::shared_ptr<ITcpClientHandler> Handler)
{
    priv->handlers.push_back(Handler);
}

void TcpClient::Run()
{
    uv_thread_t thread_id;
    uv_thread_create(&thread_id, TcpClientPrivate::onRunClient, priv.get());
}

void TcpClient::Close()
{
    uv_close((uv_handle_t*)&priv->socket, NULL);
}

void TcpClient::Write(const char* buf, int len)
{
    // todo
}
