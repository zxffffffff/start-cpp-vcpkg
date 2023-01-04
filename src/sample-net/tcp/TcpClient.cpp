#include "TcpClient.h"
#include "TcpClient_p.h"

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
    if (!priv->running)
        return;
    uv_close((uv_handle_t*)&priv->socket, TcpClientPrivate::onClose);
}

void TcpClient::Write(const char* buf, int len)
{
    // todo
}
