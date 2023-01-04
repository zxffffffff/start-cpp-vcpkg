#include "TcpServer.h"
#include "TcpServer_p.h"

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
    if (!priv->running)
        return;
    uv_close((uv_handle_t*)&priv->server, TcpServerPrivate::onClose);
}

void TcpServer::Write(SocketPtr socket_ptr, const char* buf, int len)
{

}
