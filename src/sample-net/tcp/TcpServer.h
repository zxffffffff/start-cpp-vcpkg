/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include "TcpCommon.h"

class TcpServerPrivate;
class TcpServer
{
public:
    TcpServer(const char* ip, int port, const char* tips = NULL);
    ~TcpServer();

    void SetHandleRun(HandleRun f);
    void SetHandleClose(HandleClose f);
    void SetHandleNewConn(HandleNewConn f);
    void SetHandleCloseConn(HandleCloseConn f);
    void SetHandleConnOnRead(HandleConnOnRead f);
    void SetHandleConnOnWrite(HandleConnOnWrite f);

    void Run();
    void Close();
    bool IsRunning() const;
    void Write(SocketPtr socket_ptr, const char* buf, int len);

private:
    std::unique_ptr<TcpServerPrivate> priv;
};
