/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include <algorithm>
#include <string>
#include <functional>
#include <memory>
#include <shared_mutex>
#include <future>
#include <cassert>
#include <sstream>
#include <vector>
#include <map>
#include <set>

/* 通用 alias */
using Error = std::shared_ptr<std::pair<int, std::string>>;
using Buffer = std::shared_ptr<std::vector<char>>;

inline Error MakeError(int state, const std::string& msg)
{
    return std::make_shared<std::pair<int, std::string>>(state, msg);
}

inline Error MakeSuccess()
{
    return MakeError(0, "");
}

inline Buffer MakeBuffer()
{
    return std::make_shared<std::vector<char>>();
}

inline Buffer MakeBuffer(const char* buf, int len)
{
    Buffer buffer = std::make_shared<std::vector<char>>(len);
    if (len > 0)
    {
        memcpy(buffer->data(), buf, len);
        if (buffer->at(len - 1) != '\0')
            buffer->push_back('\0');
    }
    return buffer;
}

inline Buffer MakeBuffer(const std::string& s)
{
    return MakeBuffer(s.c_str(), s.size());
}

template<class T>
inline std::future<T> MakeFuture(const T& value)
{
    std::promise<T> promise;
    std::future<T> future = promise.get_future();
    promise.set_value(value);
    return future;
}

using ConnId = size_t; /* 可以用内存地址 */

/* 线程异步通知 */
using HandleRead = std::function<void(Error, Buffer)>;
using HandleConnection = std::function<void(ConnId, Error)>;
using HandleConnectionRead = std::function<void(ConnId, Error, Buffer)>;


/* TCP抽象接口
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
