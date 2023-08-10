/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#include "libuv_tcp_client_impl.h"
#include <iostream>

inline Error MakeStatusError(int status)
{
    std::stringstream ss;
    ss << uv_err_name(status) << ":" << uv_strerror(status);
    return MakeError(status, ss.str());
}

ClientImpl::ClientImpl()
{
    eventLoop.start();
}

ClientImpl::~ClientImpl()
{
    eventLoop.stop();
}

std::future<Error> ClientImpl::Connect(const std::string& ip, int port)
{
    if (connect_promise != nullptr)
    {
        assert(false);
        return MakeFuture(MakeError(-1, __func__));
    }
    connect_promise = std::make_shared<std::promise<Error>>();
    auto future = connect_promise->get_future();

    auto onConnection = [](uv_connect_t* connection, int status)
    {
        ClientImpl* client = (ClientImpl*)connection->data;
        assert(connection == &client->connection);

        if (status < 0)
        {
            // err
            if (client->connect_promise)
            {
                client->connect_promise->set_value(MakeStatusError(status));
                client->connect_promise = nullptr;
            }
            return;
        }

        // success
        if (client->connect_promise)
        {
            client->connect_promise->set_value(MakeSuccess());
            client->connect_promise = nullptr;
        }

        auto onReadAlloc = [](uv_handle_t* socket, size_t suggested_size, uv_buf_t* buf)
        {
            buf->base = new char[suggested_size];
            buf->len = suggested_size;
        };
        auto onRead = [](uv_stream_t* socket, ssize_t nread, const uv_buf_t* buf)
        {
            ClientImpl* client = (ClientImpl*)socket->data;
            assert((uv_tcp_t*)socket == &client->socket);

            if (nread > 0)
            {
                // success
                if (client->handleRead)
                    client->handleRead(MakeSuccess(), MakeBuffer(buf->base, nread));
            }
            else if (nread < 0)
            {
                // err
                if (client->handleRead)
                    client->handleRead(MakeStatusError(nread), MakeBuffer());

                if (nread == UV_EOF)
                {
                    uv_shutdown_t* shutdown = new uv_shutdown_t;
                    shutdown->data = client;

                    auto onShutdown = [](uv_shutdown_t* shutdown, int status)
                    {
                        ClientImpl* client = (ClientImpl*)shutdown->data;
                        assert(client);
                        delete shutdown;
                    };
                    uv_shutdown(shutdown, (uv_stream_t*)socket, onShutdown);
                }
            }
            delete[] buf->base;
        };
        uv_read_start((uv_stream_t*)&client->socket, onReadAlloc, onRead);
    };

    auto runOnThread = [=]
    {
        uv_tcp_init(eventLoop.loop(), &socket);
        socket.data = this;

        uv_ip4_addr(ip.c_str(), port, &dest);

        connection.data = this;
        int status = uv_tcp_connect(&connection, &socket, (const struct sockaddr*)&dest, onConnection);
        if (status)
        {
            // err
            if (connect_promise)
            {
                connect_promise->set_value(MakeStatusError(status));
                connect_promise = nullptr;
            }
        }
    };

    eventLoop.moveToThread(runOnThread);
    return future;
}

std::future<Error> ClientImpl::Close()
{
    if (close_promise != nullptr)
    {
        assert(false);
        return MakeFuture(MakeError(-1, __func__));
    }
    close_promise = std::make_shared<std::promise<Error>>();
    auto future = close_promise->get_future();

    auto runOnThread = [=]
    {
        if (uv_is_active((uv_handle_t*)&socket))
        {
            uv_read_stop((uv_stream_t*)&socket);
        }
        if (uv_is_closing((uv_handle_t*)&socket) == 0)
        {
            auto onClose = [](uv_handle_t* socket)
            {
                ClientImpl* client = (ClientImpl*)socket->data;
                assert((uv_tcp_t*)socket == &client->socket);

                if (client->close_promise)
                {
                    client->close_promise->set_value(MakeSuccess());
                    client->close_promise = nullptr;
                }
            };
            uv_close((uv_handle_t*)&socket, onClose);
        }
    };

    eventLoop.moveToThread(runOnThread);
    return future;
}

std::future<Error> ClientImpl::Write(Buffer buffer)
{
    auto write_promise = std::make_shared<std::promise<Error>>();
    auto future = write_promise->get_future();

    {
        WriteTask task{ write_promise, buffer };
        std::lock_guard<std::mutex> guard(write_task_mutex);
        if (!write_tasks)
            write_tasks = std::make_shared<std::vector<WriteTask>>();
        write_tasks->push_back(task);
    }

    auto runOnThread = [=]
    {
        SpWriteTask write_tasks;
        {
            std::lock_guard<std::mutex> guard(write_task_mutex);
            write_tasks = this->write_tasks;
            this->write_tasks = nullptr;
        }

        if (!write_tasks)
            return;

        std::vector<char> write_buf;
        for (auto& task : *write_tasks)
            write_buf.insert(write_buf.end(), task.write_buf->begin(), task.write_buf->end());

        struct WriteReq : uv_write_t
        {
            ClientImpl* client;
            SpWriteTask write_tasks;

            WriteReq(ClientImpl* client, SpWriteTask write_tasks)
                : client(client)
                , write_tasks(write_tasks)
            {}
        };
        auto req = new WriteReq(this, write_tasks);
        auto bufs = uv_buf_init(write_buf.data(), write_buf.size());

        auto onWrite = [](uv_write_t* _req, int status)
        {
            WriteReq* req = (WriteReq*)_req;
            if (status)
            {
                // err
                for (auto& task : *req->write_tasks)
                    task.promise->set_value(MakeStatusError(status));
            }
            else
            {
                // success
                for (auto& task : *req->write_tasks)
                    task.promise->set_value(MakeSuccess());
            }
            delete req;
        };
        int status = ::uv_write(req, (uv_stream_t*)&socket, &bufs, 1, onWrite);
        if (status)
        {
            // err
            for (auto& task : *req->write_tasks)
                task.promise->set_value(MakeStatusError(status));
            delete req;
        }
    };

    eventLoop.moveToThread(runOnThread);
    return future;
}
