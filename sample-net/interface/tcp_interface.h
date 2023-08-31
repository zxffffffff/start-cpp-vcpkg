/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include "net_common.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable:4566)
#endif

/* 工作线程回调，单线程不重入 */
using HandleSocket = std::function<void(Error)>;
using HandleRead = std::function<void(Error, Buffer)>;
using HandleConn = std::function<void(ConnId, Error)>;
using HandleConnRead = std::function<void(ConnId, Error, Buffer)>;

/* TCP 抽象接口
 * 可以使用 QTcpSocket、HP-Socket、libuv 等第三方实现
 * 工作线程运行，非线程安全
 *
 * err=1 表示 EOF，不计入 LOG(ERROR)
 * connected -> read -> [handleRead:ok]
 *           -> read EOF -> shutdown -> [handleRead:EOF -> close]
 *           -> read error -> [handleRead:err -> close]
 *           -> write -> [future:ok]
 *           -> write error -> [future:err -> close]
 */
class IClient
{
public:
    virtual ~IClient() {}

    virtual void Connect(const std::string &addr, int port) = 0;
    virtual void Close() = 0;
    virtual void Write(Buffer buffer) = 0;

    virtual void SetHandleConnect(HandleSocket f) { handleConnect = f; }
    virtual void SetHandleClose(HandleSocket f) { handleClose = f; }
    virtual void SetHandleWrite(HandleSocket f) { handleWrite = f; }
    virtual void SetHandleRead(HandleRead f) { handleRead = f; }

    HandleSocket handleConnect; // 0, -100, -101
    HandleSocket handleClose;   // 0
    HandleSocket handleWrite;   // 0, -300, -301
    HandleRead handleRead;      // 0, 1, -200
};

class IServer
{
public:
    virtual ~IServer() {}

    virtual void Listen(const std::string &addr, int port) = 0;
    virtual void Close() = 0;
    virtual void Close(ConnId connId) = 0;
    virtual void Write(ConnId connId, Buffer buffer) = 0;

    virtual void SetHandleListen(HandleSocket f) { handleListen = f; }
    virtual void SetHandleClose(HandleSocket f) { handleClose = f; }
    virtual void SetHandleNewConn(HandleConn f) { handleNewConn = f; }
    virtual void SetHandleCloseConn(HandleConn f) { handleCloseConn = f; }
    virtual void SetHandleConnWrite(HandleConn f) { handleConnWrite = f; }
    virtual void SetHandleConnRead(HandleConnRead f) { handleConnRead = f; }

    HandleSocket handleListen;     // 0, -100
    HandleSocket handleClose;      // 0
    HandleConn handleNewConn;      // 0, -101, -102
    HandleConn handleCloseConn;    // 0
    HandleConn handleConnWrite;    // 0, -300, -301, -302
    HandleConnRead handleConnRead; // 0, 1, -200
};
