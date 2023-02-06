#include "TcpClient.h"
#include "TcpClient_p.h"
#include <uv.h>

TcpClient::TcpClient(const char* ip, int port)
    : priv(std::make_unique<TcpClientPrivate>(ip, port))
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

void TcpClient::Run()
{
    uv_thread_t thread_id;
    uv_thread_create(&thread_id, TcpClientPrivate::onRun, priv.get());
}

void TcpClient::Close()
{
    if (!IsRunning())
        return;
    ::uv_async_send(priv->async_close.get());
}

bool TcpClient::IsRunning() const
{
    return priv->running;
}

void TcpClient::Write(const char* buf, int len)
{
    if (!IsRunning()) {
        LOG(INFO) << "Write !IsRunning";
        return;
    }
    {
        std::lock_guard<std::mutex> guard(priv->write_mutex);
        int size = priv->write_buf.size();
        priv->write_buf.resize(size + len);
        memmove(priv->write_buf.data(), buf, len);
    }
    ::uv_async_send(priv->async_write.get());
}
