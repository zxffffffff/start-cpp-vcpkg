#pragma once
#include "TcpCommon.h"

class TcpServerPrivate;
class TcpServer
{
public:
    TcpServer(const char* ip, int port);
    ~TcpServer();

    void SetHandleRun(HandleRun f);
    void SetHandleClose(HandleClose f);
    void SetHandleNewConn(HandleNewConn f);
    void SetHandleCloseConn(HandleCloseConn f);
    void SetHandleConnOnRead(HandleConnOnRead f);

    void Run();
    void Close();

    void Write(SocketPtr socket_ptr, const char* buf, int len);

private:
    std::unique_ptr<TcpServerPrivate> priv;
};
