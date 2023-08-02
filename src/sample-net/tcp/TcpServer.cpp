/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#include "TcpServer.h"
#include "EventLoop.h"
#include <uv.h>
#include <glog/logging.h>

#define LOG_HEAD(priv) ("[[" __FUNCTION__ "]] " + priv->tips + " " + priv->ip + ":" + std::to_string(priv->port))

class TcpServerPrivate
{
private:
    std::string ip;
    int port;
    std::string tips;

    EventLoop eventLoop;
    uv_loop_t *loop = nullptr;

    std::atomic_bool running{false};
    std::atomic_bool running_success{false};
    std::shared_ptr<uv_async_t> async_close = std::make_shared<uv_async_t>();
    std::shared_ptr<uv_async_t> async_write = std::make_shared<uv_async_t>();
    std::unordered_map<SocketPtr, std::vector<char>> write_map;
    std::mutex write_mutex;

    std::shared_ptr<uv_tcp_t> socket = std::make_shared<uv_tcp_t>();
    sockaddr_in addr{0};
    const int listen_backlog = 128;
    std::list<std::shared_ptr<uv_tcp_t>> clients;
    std::vector<char> read_buf;

    bool client_exist(void *client) const
    {
        for (auto &c : clients)
        {
            if (c.get() == client)
                return true;
        }
        return false;
    }

public:
    HandleRun handleRun;
    HandleClose handleClose;
    HandleNewConn handleNewConn;
    HandleCloseConn handleCloseConn;
    HandleConnOnRead handleConnOnRead;
    HandleConnOnWrite handleConnOnWrite;

public:
    TcpServerPrivate(const char *ip, int port, const char *tips = NULL)
        : ip(ip), port(port), tips(tips ? tips : "")
    {
        eventLoop.start();
        loop = eventLoop.loop();

        async_close->data = this;
        async_write->data = this;
        uv_async_init(loop, async_close.get(), TcpServerPrivate::onEventClose);
        uv_async_init(loop, async_write.get(), TcpServerPrivate::onEventWrite);
    }

    ~TcpServerPrivate()
    {
        eventLoop.stop();
    }

    void run()
    {
        if (running)
        {
            LOG_EVERY_T(WARNING, 1) << LOG_HEAD(this) << " is running";
            return;
        }
        running = true;

        eventLoop.moveToThread([this]()
        {
            clients.clear();
            write_map.clear();
            read_buf.clear();

            uv_tcp_init(loop, socket.get());
            socket->data = this;

            uv_ip4_addr(ip.c_str(), port, &addr);

            uv_tcp_bind(socket.get(), (const struct sockaddr*)&addr, 0);

            int status = uv_listen((uv_stream_t*)socket.get(), listen_backlog, onConnection);
            if (status)
            {
                LOG(ERROR) << LOG_HEAD(this)
                    << " status=" << status
                    << " err_name=" << uv_err_name(status)
                    << " strerror=" << uv_strerror(status);
            }
            running_success = (status == 0);
            if (handleRun)
                handleRun(running_success);
        });
    }

    void close()
    {
        if (!running)
        {
            LOG_EVERY_T(WARNING, 1) << LOG_HEAD(this) << " is not running";
            return;
        }

        running = false;
        ::uv_async_send(async_close.get());
    }

    bool is_running_success() const
    {
        return running && running_success;
    }

    void write(SocketPtr socket_ptr, const char *buf, int len)
    {
        if (!running)
        {
            LOG_EVERY_T(WARNING, 1) << LOG_HEAD(this) << " is not running";
            return;
        }

        {
            std::lock_guard<std::mutex> guard(write_mutex);
            auto &write_buf = write_map[socket_ptr];
            int size = write_buf.size();
            write_buf.resize(size + len);
            memmove(write_buf.data(), buf, len);
        }
        ::uv_async_send(async_write.get());
    }

private:
    static void onEventClose(uv_async_t *handle)
    {
        auto priv = (TcpServerPrivate *)handle->data;

        if (uv_is_active((uv_handle_t *)priv->socket.get()))
        {
            uv_read_stop((uv_stream_t *)priv->socket.get());
        }
        if (uv_is_closing((uv_handle_t *)priv->socket.get()) == 0)
        {
            LOG(WARNING) << LOG_HEAD(priv) << " close socket=" << size_t(priv->socket.get());
            uv_close((uv_handle_t *)priv->socket.get(), TcpServerPrivate::onClose);
        }
        else
        {
            LOG(WARNING) << LOG_HEAD(priv) << " is_closing socket=" << size_t(priv->socket.get());
        }
    }

    static void onClose(uv_handle_t *socket)
    {
        auto priv = (TcpServerPrivate *)socket->data;
        assert((uv_tcp_t *)socket == priv->socket.get());

        if (priv->handleClose)
            priv->handleClose();
    }

    static void onConnection(uv_stream_t *socket, int status)
    {
        auto priv = (TcpServerPrivate *)socket->data;
        assert((uv_tcp_t *)socket == priv->socket.get());

        if (status < 0)
        {
            LOG(ERROR) << LOG_HEAD(priv)
                       << " status=" << status
                       << " err_name=" << uv_err_name(status)
                       << " strerror=" << uv_strerror(status);
            return;
        }

        auto client = std::make_shared<uv_tcp_t>();
        uv_tcp_init(priv->loop, client.get());
        client->data = priv;

        LOG(INFO) << LOG_HEAD(priv) << " client=" << size_t(client.get());

        if (uv_accept(socket, (uv_stream_t *)client.get()) == 0)
        {
            priv->clients.push_back(client);
            if (priv->handleNewConn)
                priv->handleNewConn(client.get());

            uv_read_start((uv_stream_t *)client.get(), onReadAlloc, onRead);
        }
        else
        {
            LOG(ERROR) << LOG_HEAD(priv) << " accept error client=" << size_t(client.get());
            uv_close((uv_handle_t *)client.get(), NULL);
        }
    }

    static void onReadAlloc(uv_handle_t *client, size_t suggested_size, uv_buf_t *buf)
    {
        auto priv = (TcpServerPrivate *)client->data;
        assert(priv->client_exist(client));

        priv->read_buf.resize(suggested_size);
        *buf = uv_buf_init(priv->read_buf.data(), suggested_size);
    }

    static void onRead(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf)
    {
        auto priv = (TcpServerPrivate *)client->data;
        assert(priv->client_exist(client));

        if (nread > 0)
        {
            LOG(INFO) << LOG_HEAD(priv) << " size=" << nread;

            if (priv->handleConnOnRead)
                priv->handleConnOnRead(client, buf->base, nread);
        }
        else if (nread < 0)
        {
            LOG(ERROR) << LOG_HEAD(priv)
                       << " nread=" << nread
                       << " err_name=" << uv_err_name(nread)
                       << " strerror=" << uv_strerror(nread);

            if (nread != UV_EOF)
            {
                if (priv->handleCloseConn)
                    priv->handleCloseConn(priv->socket.get());
            }
            else
            {
                uv_shutdown_t *shutdown = new uv_shutdown_t;
                shutdown->data = priv;
                uv_shutdown(shutdown, (uv_stream_t *)client, [](uv_shutdown_t *shutdown, int status)
                {
                    auto priv = (TcpServerPrivate*)shutdown->data;
                    LOG(WARNING) << LOG_HEAD(priv) << " shutdown socket=" << size_t(priv->socket.get());

                    if (priv->handleCloseConn)
                        priv->handleCloseConn(priv->socket.get());
                    delete shutdown; 
                });
            }
        }
    }

    static void onEventWrite(uv_async_t *handle)
    {
        auto priv = (TcpServerPrivate *)handle->data;
        if (!priv->running)
        {
            LOG_EVERY_T(WARNING, 1) << LOG_HEAD(priv) << " is not running";
            return;
        }

        std::unordered_map<SocketPtr, std::vector<char>> write_map;
        {
            std::lock_guard<std::mutex> guard(priv->write_mutex);
            std::swap(write_map, priv->write_map);
        }

        for (auto ite = write_map.begin(); ite != write_map.end(); ++ite)
        {
            auto &client = ite->first;
            auto &write_buf = ite->second;

            bool flag = false;
            for (auto ite = priv->clients.begin(); ite != priv->clients.end(); ++ite)
            {
                if (ite->get() == client)
                {
                    flag = true;
                    break;
                }
            }
            if (!flag)
                continue;

            LOG(INFO) << LOG_HEAD(priv)
                      << " handle=" << size_t(handle)
                      << " size=" << write_buf.size();

            if (write_buf.empty())
                return;

            struct WriteReq : uv_write_t
            {
                TcpServerPrivate *priv;
                SocketPtr client;
                WriteReq(TcpServerPrivate *priv, SocketPtr client) : priv(priv), client(client) {}
            };
            auto req = new WriteReq(priv, client);
            auto bufs = uv_buf_init(write_buf.data(), write_buf.size());

            int status = ::uv_write(req, (uv_stream_t *)client, &bufs, 1, [](uv_write_t *_req, int status)
                                    {
                WriteReq* req = (WriteReq*)_req;
                auto priv = req->priv;
                auto client = req->client;
                if (status)
                {
                    LOG(ERROR) << LOG_HEAD(priv)
                        << " status=" << status
                        << " err_name=" << uv_err_name(status)
                        << " strerror=" << uv_strerror(status);
                }
                if (priv->handleConnOnWrite)
                    priv->handleConnOnWrite(client, status);
                delete req; });

            if (status)
            {
                LOG(ERROR) << LOG_HEAD(priv)
                           << " status=" << status
                           << " err_name=" << uv_err_name(status)
                           << " strerror=" << uv_strerror(status);

                if (priv->handleConnOnWrite)
                    priv->handleConnOnWrite(client, status);
                delete req;
            }
        }
    }
};


TcpServer::TcpServer(const char* ip, int port, const char* tips)
    : priv(std::make_unique<TcpServerPrivate>(ip, port, tips))
{
}

TcpServer::~TcpServer()
{
}

void TcpServer::SetHandleRun(HandleRun f)
{
    priv->handleRun = f;
}

void TcpServer::SetHandleClose(HandleClose f)
{
    priv->handleClose = f;
}

void TcpServer::SetHandleNewConn(HandleNewConn f)
{
    priv->handleNewConn = f;
}

void TcpServer::SetHandleCloseConn(HandleCloseConn f)
{
    priv->handleCloseConn = f;
}

void TcpServer::SetHandleConnOnRead(HandleConnOnRead f)
{
    priv->handleConnOnRead = f;
}

void TcpServer::SetHandleConnOnWrite(HandleConnOnWrite f)
{
    priv->handleConnOnWrite = f;
}

void TcpServer::Run()
{
    priv->run();
}

void TcpServer::Close()
{
    priv->close();
}

bool TcpServer::IsRunning() const
{
    return priv->is_running_success();
}

void TcpServer::Write(SocketPtr socket_ptr, const char* buf, int len)
{
    priv->write(socket_ptr, buf, len);
}
