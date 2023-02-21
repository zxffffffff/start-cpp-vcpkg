/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/

#include "TcpServer.h"
#include "TcpServer_p.h"
#include <uv.h>

TcpServer::TcpServer(const char* ip, int port)
    : priv(std::make_unique<TcpServerPrivate>(ip, port))
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

void TcpServer::Run()
{
    uv_thread_t thread_id;
    uv_thread_create(&thread_id, TcpServerPrivate::onRun, priv.get());
}

void TcpServer::Close()
{
    if (!IsRunning())
        return;
    ::uv_async_send(priv->async_close.get());
}

bool TcpServer::IsRunning() const
{
    return priv->running;
}

void TcpServer::Write(SocketPtr socket_ptr, const char* buf, int len)
{
    if (!IsRunning()) {
        LOG(INFO) << "Write !IsRunning";
        return;
    }
    {
        std::lock_guard<std::mutex> guard(priv->write_mutex);
        auto& write_buf = priv->write_map[socket_ptr];
        int size = write_buf.size();
        write_buf.resize(size + len);
        memmove(write_buf.data(), buf, len);
    }
    ::uv_async_send(priv->async_write.get());
}
