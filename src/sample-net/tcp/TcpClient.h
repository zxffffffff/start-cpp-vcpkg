#pragma once
#include "TcpCommon.h"

class TcpClientPrivate;
class TcpClient
{
public:
    TcpClient(const char* ip, int port);
    ~TcpClient();

    void SetHandleRun(HandleRun f);
    void SetHandleClose(HandleClose f);
    void SetHandleNewConn(HandleNewConn f);
    void SetHandleCloseConn(HandleCloseConn f);
    void SetHandleConnOnRead(HandleConnOnRead f);

    void Run();
    void Close();
    bool IsRunning() const;

    void Write(const char* buf, int len);

private:
    std::unique_ptr<TcpClientPrivate> priv;
};
