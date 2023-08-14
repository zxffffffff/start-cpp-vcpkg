/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include "../tcp_interface.h"
#include "libuv_event_loop.h"

class ServerImpl : public IServer
{
public:
    /* 线程事件循环 */
    EventLoop eventLoop;

    /* socket */
    uv_tcp_t socket{ 0 };
    sockaddr_in addr{ 0 };
    const int listen_backlog = 128;
    std::vector<ConnId> connections; /* uv_tcp_t* */

    /* 线程通信 */
    std::shared_ptr<std::promise<Error>> listen_promise;
    std::shared_ptr<std::promise<Error>> close_promise;

    struct WriteTask
    {
        std::shared_ptr<std::promise<Error>> promise;
        ConnId connId;
        Buffer write_buf;
    };
    using SpWriteTask = std::shared_ptr<std::vector<WriteTask>>;
    SpWriteTask write_tasks;
    std::mutex write_task_mutex;

public:
    ServerImpl()
    {
        eventLoop.start();
    }

    virtual ~ServerImpl()
    {
        assert(connections.empty());
        eventLoop.stop();
    }

    virtual std::future<Error> Listen(const std::string& ip, int port) override
    {
        if (listen_promise != nullptr)
        {
            assert(false);
            return MakeFuture(MakeError(-1, __func__));
        }
        listen_promise = std::make_shared<std::promise<Error>>();
        auto future = listen_promise->get_future();
        eventLoop.moveToThread([=] { ListenOnEvent(ip, port); });
        return future;
    }

    virtual std::future<Error> Close() override
    {
        if (close_promise != nullptr)
        {
            assert(false);
            return MakeFuture(MakeError(-1, __func__));
        }
        close_promise = std::make_shared<std::promise<Error>>();
        auto future = close_promise->get_future();
        eventLoop.moveToThread([=] { CloseOnEvent(); });
        return future;
    }

    virtual std::future<Error> Write(ConnId connId, Buffer buffer) override
    {
        auto write_promise = std::make_shared<std::promise<Error>>();
        auto future = write_promise->get_future();
        {
            WriteTask task{ write_promise, connId, buffer };
            std::lock_guard<std::mutex> guard(write_task_mutex);
            if (!write_tasks)
                write_tasks = std::make_shared<std::vector<WriteTask>>();
            write_tasks->push_back(task);
        }
        eventLoop.moveToThread([=] { WriteOnEvent(connId, buffer); });
        return future;
    }

private:
    /* 网络线程 */
    void ListenOnEvent(const std::string& ip, int port)
    {
        uv_tcp_init(eventLoop.loop(), &socket);
        socket.data = this;

        uv_ip4_addr(ip.c_str(), port, &addr);

        uv_tcp_bind(&socket, (const struct sockaddr*)&addr, 0);
        int status = uv_listen((uv_stream_t*)&socket, listen_backlog, onConnection);
        if (status)
        {
            // err
            if (listen_promise)
            {
                listen_promise->set_value(MakeStatusError(status));
                listen_promise = nullptr;
            }
            return;
        }

        // success
        if (listen_promise)
        {
            listen_promise->set_value(MakeSuccess());
            listen_promise = nullptr;
        }
    }

    static void onConnection(uv_stream_t* socket, int status)
    {
        ServerImpl* server = (ServerImpl*)socket->data;
        assert((uv_tcp_t*)socket == &server->socket);

        if (status < 0)
        {
            // err
            if (server->handleNewConn)
                server->handleNewConn(0, MakeStatusError(status));
            return;
        }

        uv_tcp_t* connection = new uv_tcp_t{ 0 };
        uv_tcp_init(server->eventLoop.loop(), connection);
        connection->data = server;

        int status2 = uv_accept(socket, (uv_stream_t*)connection);
        if (status2)
        {
            // err
            auto onClose = [](uv_handle_t* connection)
            {
                delete connection;
            };
            uv_close((uv_handle_t*)connection, onClose);

            if (server->handleNewConn)
                server->handleNewConn(0, MakeStatusError(status2));
            return;
        }

        // success
        server->connections.push_back((ConnId)connection);

        if (server->handleNewConn)
            server->handleNewConn((ConnId)connection, MakeSuccess());

        uv_read_start((uv_stream_t*)connection, onReadAlloc, onRead);
    }

    static void onReadAlloc(uv_handle_t* connection, size_t suggested_size, uv_buf_t* buf)
    {
        buf->base = new char[suggested_size];
        buf->len = suggested_size;
    }

    static void onRead(uv_stream_t* connection, ssize_t nread, const uv_buf_t* buf)
    {
        ServerImpl* server = (ServerImpl*)connection->data;
        assert(std::find(server->connections.begin(), server->connections.end(), (ConnId)connection) != server->connections.end());

        if (nread > 0)
        {
            // success
            if (server->handleConnRead)
                server->handleConnRead((ConnId)connection, MakeSuccess(), MakeBuffer(buf->base, nread));
        }
        else if (nread < 0)
        {
            // err
            if (server->handleConnRead)
                server->handleConnRead((ConnId)connection, MakeStatusError(nread), MakeBuffer());

            if (nread == UV_EOF)
            {
                uv_shutdown_t* shutdown = new uv_shutdown_t;
                shutdown->data = connection;

                auto onShutdown = [](uv_shutdown_t* shutdown, int status)
                {
                    uv_tcp_t* connection = (uv_tcp_t*)shutdown->data;
                    ServerImpl* server = (ServerImpl*)connection->data;
                    assert(connection && server);
                    server->connections.erase(std::find(server->connections.begin(), server->connections.end(), (ConnId)connection));

                    if (server->handleCloseConn)
                        server->handleCloseConn((ConnId)connection, MakeStatusError(status));

                    delete shutdown;
                };
                uv_shutdown(shutdown, (uv_stream_t*)connection, onShutdown);
            }
        }
        delete[] buf->base;
    }

    void CloseOnEvent()
    {
        if (uv_is_active((uv_handle_t*)&socket))
        {
            uv_read_stop((uv_stream_t*)&socket);
        }
        if (uv_is_closing((uv_handle_t*)&socket) == 0)
        {
            auto onClose = [](uv_handle_t* socket)
            {
                ServerImpl* server = (ServerImpl*)socket->data;
                assert((uv_tcp_t*)socket == &server->socket);

                if (server->close_promise)
                {
                    server->close_promise->set_value(MakeSuccess());
                    server->close_promise = nullptr;
                }
            };
            uv_close((uv_handle_t*)&socket, onClose);
        }
        else
        {
            assert(false);
        }
    }

    struct WriteReq : uv_write_t
    {
        ServerImpl* server;
        SpWriteTask write_tasks;
        ConnId connId;

        WriteReq(ServerImpl* server, SpWriteTask write_tasks, ConnId connId)
            : server(server)
            , write_tasks(write_tasks)
            , connId(connId)
        {}
    };
    void WriteOnEvent(ConnId connId, Buffer buffer)
    {

        SpWriteTask write_tasks;
        {
            std::lock_guard<std::mutex> guard(write_task_mutex);
            write_tasks = this->write_tasks;
            this->write_tasks = nullptr;
        }

        if (!write_tasks)
            return;

        std::map<ConnId, std::vector<char>> map_write_buf;
        for (auto& task : *write_tasks)
        {
            auto& write_buf = map_write_buf[task.connId];
            write_buf.insert(write_buf.end(), task.write_buf->begin(), task.write_buf->end());
        }

        for (auto ite : map_write_buf)
        {
            ConnId connId = ite.first;
            auto& write_buf = ite.second;

            if (connId == 0)
            {
                assert(false);
                continue;
            }

            auto req = new WriteReq(this, write_tasks, connId);
            auto bufs = uv_buf_init(write_buf.data(), write_buf.size());
            int status = ::uv_write(req, (uv_stream_t*)connId, &bufs, 1, onWrite);
            if (status)
            {
                // err
                for (auto& task : *req->write_tasks)
                {
                    if (task.connId == req->connId)
                        task.promise->set_value(MakeStatusError(status));
                }
                delete req;
            }
        }
    }

    static void onWrite(uv_write_t* _req, int status)
    {
        WriteReq* req = (WriteReq*)_req;
        if (status)
        {
            // err
            for (auto& task : *req->write_tasks)
            {
                if (task.connId == req->connId)
                    task.promise->set_value(MakeStatusError(status));
            }
        }
        else
        {
            // success
            for (auto& task : *req->write_tasks)
            {
                if (task.connId == req->connId)
                    task.promise->set_value(MakeSuccess());
            }
        }
        delete req;
    }
};
