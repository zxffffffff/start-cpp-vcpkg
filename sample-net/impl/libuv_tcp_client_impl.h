/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include "libuv_event_loop.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable:4566)
#endif

class ClientImpl : public IClient
{
public:
    /* 线程事件循环 */
    EventLoop eventLoop;

    /* socket */
    uv_tcp_t socket{0};
    sockaddr_in dest{0};

    /* 线程通信 */
    std::shared_ptr<std::promise<Error>> connect_promise;
    std::shared_ptr<std::promise<Error>> close_promise;

public:
    ClientImpl()
    {
        eventLoop.start();
    }

    virtual ~ClientImpl()
    {
        eventLoop.stop();
    }

    virtual void Connect(const std::string &ip, int port) override
    {
        assert(handleConnect); /* 必须初始化 handler */
        assert(handleClose);
        assert(handleWrite);
        assert(handleRead);

        if (eventLoop.thisIsLoop())
        {
            ConnectOnEvent(ip, port);
        }
        else
        {
            eventLoop.moveToThread([=]
                                   { ConnectOnEvent(ip, port); });
        }
    }

    virtual void Close() override
    {
        if (eventLoop.thisIsLoop())
        {
            CloseOnEvent();
        }
        else
        {
            eventLoop.moveToThread([=]
                                   { CloseOnEvent(); });
        }
    }

    virtual void Write(Buffer buffer) override
    {
        if (eventLoop.thisIsLoop())
        {
            WriteOnEvent(buffer);
        }
        else
        {
            eventLoop.moveToThread([=]
                                   { WriteOnEvent(buffer); });
        }
    }

private:
    /* 网络线程 */
    void ConnectOnEvent(const std::string &ip, int port)
    {
        uv_tcp_init(eventLoop.loop(), &socket);
        socket.data = this;

        int addr_status = uv_ip4_addr(ip.c_str(), port, &dest);
        if (addr_status)
        {
            // EINVAL
            // std::string addr_err = uv_err_name(addr_status);

            /* 支持 DNS 解析域名 */
            struct addrinfo hints;
            hints.ai_family = PF_INET;
            hints.ai_socktype = SOCK_STREAM;
            hints.ai_protocol = IPPROTO_TCP;
            hints.ai_flags = 0;

            uv_getaddrinfo_t resolver;
            int r = uv_getaddrinfo(eventLoop.loop(), &resolver, NULL, ip.c_str(), NULL, &hints);
            if (r)
            {
                // err
                handleConnect(MakeStatusError(-102, r));
                return;
            }

            struct addrinfo *res = resolver.addrinfo;
            char dns_ip[17] = {'\0'};
            uv_ip4_name((struct sockaddr_in *)res->ai_addr, dns_ip, 16);
            addr_status = uv_ip4_addr(dns_ip, port, &dest);
            uv_freeaddrinfo(res);
        }

        uv_connect_t *connection = new uv_connect_t{0};
        connection->data = this;
        int status = uv_tcp_connect(connection, &socket, (const struct sockaddr *)&dest, onConnection);
        if (status)
        {
            // err
            handleConnect(MakeStatusError(-100, status));
            return;
        }
    }

    static void onConnection(uv_connect_t *connection, int status)
    {
        ClientImpl *client = (ClientImpl *)connection->data;
        assert(client);
        delete connection;

        if (status < 0)
        {
            // err
            client->handleConnect(MakeStatusError(-101, status));
            return;
        }

        // success
        client->handleConnect(MakeSuccess());

        uv_tcp_nodelay(&client->socket, 1); /* 禁用Nagle算法，减少数据传输的延迟 */
        uv_read_start((uv_stream_t *)&client->socket, onReadAlloc, onRead);
    }

    static void onReadAlloc(uv_handle_t *socket, size_t suggested_size, uv_buf_t *buf)
    {
        buf->base = new char[suggested_size];
        buf->len = suggested_size;
    }

    static void onRead(uv_stream_t *socket, ssize_t nread, const uv_buf_t *buf)
    {
        ClientImpl *client = (ClientImpl *)socket->data;
        assert((uv_tcp_t *)socket == &client->socket);

        if (nread > 0)
        {
            // success
            client->handleRead(MakeSuccess(), MakeBuffer(buf->base, nread));
        }
        else if (nread == UV_EOF)
        {
            uv_shutdown_t *shutdown = new uv_shutdown_t{0};
            shutdown->data = client;

            auto onShutdown = [](uv_shutdown_t *shutdown, int status)
            {
                ClientImpl *client = (ClientImpl *)shutdown->data;
                assert(client);
                delete shutdown;

                // EOF
                client->handleRead(MakeStatusError(1, UV_EOF), MakeBuffer());
            };
            uv_shutdown(shutdown, (uv_stream_t *)socket, onShutdown);
        }
        else if (nread < 0)
        {
            // error
            client->handleRead(MakeStatusError(-200, nread), MakeBuffer());
        }
        delete[] buf->base;
    }

    struct WriteReq : uv_write_t
    {
        ClientImpl *client;
        Buffer buffer;

        WriteReq(ClientImpl *client, Buffer buffer)
            : client(client), buffer(buffer)
        {
        }
    };
    void WriteOnEvent(Buffer buffer)
    {
        auto req = new WriteReq(this, buffer);
        auto bufs = uv_buf_init(buffer->data(), buffer->size());
        int status = ::uv_write(req, (uv_stream_t *)&socket, &bufs, 1, onWrite);
        if (status)
        {
            // err
            handleWrite(MakeStatusError(-300, status));
            delete req;
        }
    }

    static void onWrite(uv_write_t *_req, int status)
    {
        WriteReq *req = (WriteReq *)_req;
        ClientImpl *client = (ClientImpl *)req->client;
        assert(client);

        if (status)
        {
            // err
            client->handleWrite(MakeStatusError(-301, status));
        }
        else
        {
            // success
            client->handleWrite(MakeSuccess());
        }
        delete req;
    }

    void CloseOnEvent()
    {
        if (uv_is_active((uv_handle_t *)&socket))
        {
            uv_read_stop((uv_stream_t *)&socket);
        }
        if (uv_is_closing((uv_handle_t *)&socket) == 0)
        {
            auto onClose = [](uv_handle_t *socket)
            {
                ClientImpl *client = (ClientImpl *)socket->data;
                assert((uv_tcp_t *)socket == &client->socket);

                client->handleClose(MakeSuccess());
            };
            uv_close((uv_handle_t *)&socket, onClose);
        }
    }
};
