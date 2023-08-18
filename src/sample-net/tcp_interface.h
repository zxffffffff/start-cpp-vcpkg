/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include "net_common.h"

/* 线程异步通知 */
using HandleRead = std::function<void(Error, Buffer)>;
using HandleConnection = std::function<void(ConnId, Error)>;
using HandleConnectionRead = std::function<void(ConnId, Error, Buffer)>;

/* TCP 抽象接口
 * 可以使用 QTcpSocket、HP-Socket、libuv 等第三方实现
 * 工作线程运行，非线程安全
 */
class IServer
{
public:
    virtual ~IServer() {}

    virtual std::future<Error> Listen(const std::string& addr, int port) = 0;
    virtual std::future<Error> Close() = 0;
    virtual std::future<Error> Write(ConnId connId, Buffer buffer) = 0;

    virtual void SetHandleNewConn(HandleConnection f) { handleNewConn = f; }
    virtual void SetHandleCloseConn(HandleConnection f) { handleCloseConn = f; }
    virtual void SetHandleConnRead(HandleConnectionRead f) { handleConnRead = f; }
   
    HandleConnection handleNewConn;
    HandleConnection handleCloseConn;
    HandleConnectionRead handleConnRead;
};

class IClient
{
public:
    virtual ~IClient() {}

    virtual std::future<Error> Connect(const std::string& addr, int port) = 0;
    virtual std::future<Error> Close() = 0;
    virtual std::future<Error> Write(Buffer buffer) = 0;

    virtual void SetHandleRead(HandleRead f) { handleRead = f; }

    HandleRead handleRead;
};
