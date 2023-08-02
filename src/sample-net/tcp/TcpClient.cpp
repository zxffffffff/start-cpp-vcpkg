/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#include "TcpClient.h"
#include "EventLoop.h"
#include <uv.h>
#include <glog/logging.h>

#define LOG_HEAD(priv) ("[[" __FUNCTION__ "]] " + priv->tips + " " + priv->ip + ":" + std::to_string(priv->port))

class TcpClientPrivate
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
    std::vector<char> write_buf;
    std::mutex write_mutex;

    std::shared_ptr<uv_tcp_t> socket = std::make_shared<uv_tcp_t>();
    sockaddr_in dest{0};
    std::shared_ptr<uv_connect_t> connect = std::make_shared<uv_connect_t>();
    std::vector<char> read_buf;

public:
    HandleRun handleRun;
    HandleClose handleClose;
    HandleNewConn handleNewConn;
    HandleCloseConn handleCloseConn;
    HandleConnOnRead handleConnOnRead;
    HandleConnOnWrite handleConnOnWrite;

public:
    TcpClientPrivate(const char *ip, int port, const char *tips = NULL)
        : ip(ip), port(port), tips(tips ? tips : "")
    {
        eventLoop.start();
        loop = eventLoop.loop();

        async_close->data = this;
        async_write->data = this;
        uv_async_init(loop, async_close.get(), TcpClientPrivate::onEventClose);
        uv_async_init(loop, async_write.get(), TcpClientPrivate::onEventWrite);
    }

    ~TcpClientPrivate()
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
            write_buf.clear();
            read_buf.clear();

            uv_tcp_init(loop, socket.get());
            socket->data = this;

            uv_ip4_addr(ip.c_str(), port, &dest);

            connect->data = this;
            int status = uv_tcp_connect(connect.get(), socket.get(), (const struct sockaddr*)&dest, onConnection);
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

    void write(const char *buf, int len)
    {
        if (!running)
        {
            LOG_EVERY_T(WARNING, 1) << LOG_HEAD(this) << " is not running";
            return;
        }

        {
            std::lock_guard<std::mutex> guard(write_mutex);
            int size = write_buf.size();
            write_buf.resize(size + len);
            memmove(write_buf.data(), buf, len);
        }
        ::uv_async_send(async_write.get());
    }

private:
    static void onEventClose(uv_async_t *handle)
    {
        auto priv = (TcpClientPrivate *)handle->data;

        if (uv_is_active((uv_handle_t *)priv->socket.get()))
        {
            uv_read_stop((uv_stream_t *)priv->socket.get());
        }
        if (uv_is_closing((uv_handle_t *)priv->socket.get()) == 0)
        {
            LOG(WARNING) << LOG_HEAD(priv) << " close socket=" << size_t(priv->socket.get());
            uv_close((uv_handle_t *)priv->socket.get(), TcpClientPrivate::onClose);
        }
        else
        {
            LOG(WARNING) << LOG_HEAD(priv) << " is_closing socket=" << size_t(priv->socket.get());
        }
    }

    static void onClose(uv_handle_t *socket)
    {
        auto priv = (TcpClientPrivate *)socket->data;
        assert((uv_tcp_t *)socket == priv->socket.get());

        if (priv->handleClose)
            priv->handleClose();
    }

    static void onConnection(uv_connect_t *connect, int status)
    {
        auto priv = (TcpClientPrivate *)connect->data;
        assert(connect == priv->connect.get());

        if (status < 0)
        {
            LOG(ERROR) << LOG_HEAD(priv)
                       << " status=" << status
                       << " err_name=" << uv_err_name(status)
                       << " strerror=" << uv_strerror(status);
            return;
        }

        LOG(INFO) << LOG_HEAD(priv) << " connect=" << size_t(connect);

        if (priv->handleNewConn)
            priv->handleNewConn(priv->socket.get());

        uv_read_start((uv_stream_t *)priv->socket.get(), onReadAlloc, onRead);
    }

    static void onReadAlloc(uv_handle_t *socket, size_t suggested_size, uv_buf_t *buf)
    {
        auto priv = (TcpClientPrivate *)socket->data;
        assert((uv_tcp_t *)socket == priv->socket.get());

        priv->read_buf.resize(suggested_size);
        *buf = uv_buf_init(priv->read_buf.data(), suggested_size);
    }

    static void onRead(uv_stream_t *socket, ssize_t nread, const uv_buf_t *buf)
    {
        auto priv = (TcpClientPrivate *)socket->data;
        assert((uv_tcp_t *)socket == priv->socket.get());

        if (nread > 0)
        {
            LOG(INFO) << LOG_HEAD(priv) << " size=" << nread;

            if (priv->handleConnOnRead)
                priv->handleConnOnRead(socket, buf->base, nread);
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
                uv_shutdown(shutdown, (uv_stream_t *)socket, [](uv_shutdown_t *shutdown, int status)
                {
                    auto priv = (TcpClientPrivate*)shutdown->data;
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
        auto priv = (TcpClientPrivate *)handle->data;
        if (!priv->running)
        {
            LOG_EVERY_T(WARNING, 1) << LOG_HEAD(priv) << " is not running";
            return;
        }

        std::vector<char> write_buf;
        {
            std::lock_guard<std::mutex> guard(priv->write_mutex);
            std::swap(write_buf, priv->write_buf);
        }

        LOG(INFO) << LOG_HEAD(priv)
                  << " handle=" << size_t(handle)
                  << " size=" << write_buf.size();

        if (write_buf.empty())
            return;

        struct WriteReq : uv_write_t
        {
            TcpClientPrivate *priv;
            WriteReq(TcpClientPrivate *priv) : priv(priv) {}
        };
        auto req = new WriteReq(priv);
        auto bufs = uv_buf_init(write_buf.data(), write_buf.size());

        int status = ::uv_write(req, (uv_stream_t *)priv->socket.get(), &bufs, 1, [](uv_write_t *_req, int status)
        {
            WriteReq* req = (WriteReq*)_req;
            auto priv = req->priv;
            if (status)
            {
                LOG(ERROR) << LOG_HEAD(priv)
                    << " status=" << status
                    << " err_name=" << uv_err_name(status)
                    << " strerror=" << uv_strerror(status);
            }
            if (priv->handleConnOnWrite)
                priv->handleConnOnWrite(priv->socket.get(), status);
            delete req; 
        });

        if (status)
        {
            LOG(ERROR) << LOG_HEAD(priv)
                       << " status=" << status
                       << " err_name=" << uv_err_name(status)
                       << " strerror=" << uv_strerror(status);

            if (priv->handleConnOnWrite)
                priv->handleConnOnWrite(priv->socket.get(), status);
            delete req;
        }
    }
};


TcpClient::TcpClient(const char* ip, int port, const char* tips)
    : priv(std::make_unique<TcpClientPrivate>(ip, port, tips))
{
}

TcpClient::~TcpClient()
{
}

void TcpClient::SetHandleRun(HandleRun f)
{
    priv->handleRun = f;
}

void TcpClient::SetHandleClose(HandleClose f)
{
    priv->handleClose = f;
}

void TcpClient::SetHandleNewConn(HandleNewConn f)
{
    priv->handleNewConn = f;
}

void TcpClient::SetHandleCloseConn(HandleCloseConn f)
{
    priv->handleCloseConn = f;
}

void TcpClient::SetHandleConnOnRead(HandleConnOnRead f)
{
    priv->handleConnOnRead = f;
}

void TcpClient::SetHandleConnOnWrite(HandleConnOnWrite f)
{
    priv->handleConnOnWrite = f;
}

void TcpClient::Run()
{
    priv->run();
}

void TcpClient::Close()
{
    priv->close();
}

bool TcpClient::IsRunning() const
{
    return priv->is_running_success();
}

void TcpClient::Write(const char* buf, int len)
{
    priv->write(buf, len);
}
