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

class ServerImpl : public IServer
{
public:
    /* 线程事件循环 */
    EventLoop eventLoop;

    /* socket */
    uv_tcp_t socket{0};
    sockaddr_in addr{0};
    const int listen_backlog = 128;
    std::set<ConnId> connections; /* uv_tcp_t* */

public:
    ServerImpl()
    {
        eventLoop.start();
    }

    virtual ~ServerImpl()
    {
        assert(connections.empty()); /* 不应该出现 */
        eventLoop.stop();
    }

    virtual void Listen(const std::string &ip, int port) override
    {
        assert(handleListen); /* 必须初始化 handler */
        assert(handleClose);
        assert(handleNewConn);
        assert(handleCloseConn);
        assert(handleConnWrite);
        assert(handleConnRead);

        if (eventLoop.thisIsLoop())
        {
            ListenOnEvent(ip, port);
        }
        else
        {
            eventLoop.moveToThread([=]
                                   { ListenOnEvent(ip, port); });
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

    virtual void Close(ConnId connId) override
    {
        if (eventLoop.thisIsLoop())
        {
            CloseOnEvent(connId);
        }
        else
        {
            eventLoop.moveToThread([=]
                                   { CloseOnEvent(connId); });
        }
    }

    virtual void Write(ConnId connId, Buffer buffer) override
    {
        if (eventLoop.thisIsLoop())
        {
            WriteOnEvent(connId, buffer);
        }
        else
        {
            eventLoop.moveToThread([=]
                                   { WriteOnEvent(connId, buffer); });
        }
    }

private:
    /* 网络线程 */
    void ListenOnEvent(const std::string &ip, int port)
    {
        uv_tcp_init(eventLoop.loop(), &socket);
        socket.data = this;

        uv_ip4_addr(ip.c_str(), port, &addr);

        uv_tcp_bind(&socket, (const struct sockaddr *)&addr, 0);
        int status = uv_listen((uv_stream_t *)&socket, listen_backlog, onConnection);
        if (status)
        {
            // err
            handleListen(MakeStatusError(-100, status));
            return;
        }

        // success
        handleListen(MakeSuccess());
    }

    static void onConnection(uv_stream_t *socket, int status)
    {
        ServerImpl *server = (ServerImpl *)socket->data;
        assert((uv_tcp_t *)socket == &server->socket);

        if (status < 0)
        {
            // err
            server->handleNewConn(0, MakeStatusError(-101, status));
            return;
        }

        uv_tcp_t *connection = new uv_tcp_t{0};
        uv_tcp_init(server->eventLoop.loop(), connection);
        connection->data = server;

        int status2 = uv_accept(socket, (uv_stream_t *)connection);
        if (status2)
        {
            // err
            auto onClose = [](uv_handle_t *connection)
            {
                delete connection;
            };
            uv_close((uv_handle_t *)connection, onClose);

            server->handleNewConn((ConnId)connection, MakeStatusError(-102, status2));
            return;
        }

        // success
        uv_tcp_nodelay(connection, 1); /* 禁用Nagle算法，减少数据传输的延迟 */
        uv_read_start((uv_stream_t *)connection, onReadAlloc, onRead);

        assert(server->connections.find((ConnId)connection) == server->connections.end());
        server->connections.insert((ConnId)connection);

        server->handleNewConn((ConnId)connection, MakeSuccess());
    }

    static void onReadAlloc(uv_handle_t *connection, size_t suggested_size, uv_buf_t *buf)
    {
        buf->base = new char[suggested_size];
        buf->len = suggested_size;
    }

    static void onRead(uv_stream_t *connection, ssize_t nread, const uv_buf_t *buf)
    {
        ServerImpl *server = (ServerImpl *)connection->data;
        assert(server->connections.find((ConnId)connection) != server->connections.end());

        if (nread > 0)
        {
            // success
            server->handleConnRead((ConnId)connection, MakeSuccess(), MakeBuffer(buf->base, nread));
        }
        else if (nread == UV_EOF)
        {
            uv_shutdown_t *shutdown = new uv_shutdown_t{0};
            shutdown->data = connection;
            auto onShutdown = [](uv_shutdown_t *shutdown, int status)
            {
                uv_tcp_t *connection = (uv_tcp_t *)shutdown->data;
                ServerImpl *server = (ServerImpl *)connection->data;
                assert(connection && server);
                delete shutdown;

                // EOF
                server->handleConnRead((ConnId)connection, MakeStatusError(1, UV_EOF), MakeBuffer());
            };
            uv_shutdown(shutdown, (uv_stream_t *)connection, onShutdown);
        }
        else if (nread < 0)
        {
            // error
            server->handleConnRead((ConnId)connection, MakeStatusError(-200, nread), MakeBuffer());
        }
        delete[] buf->base;
    }

    struct WriteReq : uv_write_t
    {
        ServerImpl *server;
        ConnId connId;
        Buffer buffer;

        WriteReq(ServerImpl *server, ConnId connId, Buffer buffer)
            : server(server), connId(connId), buffer(buffer)
        {
        }
    };
    void WriteOnEvent(ConnId connId, Buffer buffer)
    {
        if (connections.find(connId) == connections.end())
        {
            /* connId 已断开，多线程问题 */
            handleConnWrite(connId, MakeError(-302, "connection no found"));
            return;
        }

        auto req = new WriteReq(this, connId, buffer);
        auto bufs = uv_buf_init(buffer->data(), buffer->size());
        int status = ::uv_write(req, (uv_stream_t *)connId, &bufs, 1, onWrite);
        if (status)
        {
            // err
            handleConnWrite(req->connId, MakeStatusError(-300, status));
            delete req;
        }
    }

    static void onWrite(uv_write_t *_req, int status)
    {
        WriteReq *req = (WriteReq *)_req;
        ServerImpl *server = (ServerImpl *)req->server;
        assert(server);

        if (status)
        {
            // err
            server->handleConnWrite(req->connId, MakeStatusError(-301, status));
        }
        else
        {
            // success
            server->handleConnWrite(req->connId, MakeSuccess());
        }
        delete req;
    }

    void CloseOnEvent(ConnId connId)
    {
        if (connections.find(connId) == connections.end())
        {
            /* connId 已断开，多线程问题 */
            return;
        }

        uv_handle_t *connection = (uv_handle_t *)connId;
        if (uv_is_active((uv_handle_t *)connection))
        {
            uv_read_stop((uv_stream_t *)connection);
        }
        if (uv_is_closing((uv_handle_t *)connection) == 0)
        {
            auto onClose = [](uv_handle_t *connection)
            {
                ServerImpl *server = (ServerImpl *)connection->data;
                assert(server);
                server->connections.erase((ConnId)connection);

                server->handleCloseConn((ConnId)connection, MakeSuccess());
                delete connection;
            };
            uv_close((uv_handle_t *)connection, onClose);
        }
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
                ServerImpl *server = (ServerImpl *)socket->data;
                assert((uv_tcp_t *)socket == &server->socket);

                server->handleClose(MakeSuccess());
            };
            uv_close((uv_handle_t *)&socket, onClose);
        }
    }
};
