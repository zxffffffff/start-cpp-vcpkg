/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include "tcp_server.h"
#include "interface/http_interface.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable:4566)
#endif

/* 线程异步回调，注意线程安全 */
using ServerResponseCbk = std::function<void(std::string)>;
using HandleServerRequest = std::function<void(ConnId, Error, const HttpRequest &, ServerResponseCbk)>;

template <class IHttpParserImpl, class ITcpServerImpl, class IThreadPoolImpl>
class HttpServer : public TcpServer<ITcpServerImpl, IThreadPoolImpl>
{
protected:
    using Super = TcpServer<ITcpServerImpl, IThreadPoolImpl>;

private:
    HandleServerRequest handleServerRequest;

    struct Connection
    {
        Buffer recv;
        std::unique_ptr<IHttpParser> parser = std::make_unique<IHttpParserImpl>();
        mutable std::shared_mutex connMutex;
    };
    std::map<ConnId, std::shared_ptr<Connection>> conns;

public:
    HttpServer(const std::string &addr = "127.0.0.1", int port = 12345, const std::string &tips = "HttpServer")
        : Super(addr, port, tips)
    {
    }

    virtual ~HttpServer()
    {
    }

    /* 线程异步回调，注意线程安全 */
    void SetHandleServerRequest(HandleServerRequest f) { handleServerRequest = f; }

private:
    /* 单线程同步，不可重入 */
    virtual void OnNewConn(ConnId connId, Error err) override
    {
        if (err->first == 0)
            conns[connId] = std::make_shared<Connection>();

        Super::OnNewConn(connId, err);
    }

    virtual void OnCloseConn(ConnId connId, Error err) override
    {
        Super::OnCloseConn(connId, err);

        if (err->first == 0)
            conns.erase(conns.find(connId));
    }

    virtual void OnConnRead(ConnId connId, Error err, Buffer buffer) override
    {
        if (handleServerRequest)
        {
            std::shared_ptr<Connection> conn = conns[connId];
            Super::threadPool->MoveToThread([=]
                                            { OnConnRequest(connId, conn, err, buffer); });
        }

        Super::OnConnRead(connId, err, buffer);
    }

    /* 线程异步处理 http request */
    void OnConnRequest(ConnId connId, std::shared_ptr<Connection> conn, Error err, Buffer buffer)
    {
        HttpRequest req;
        auto cbkWrite = [=](std::string resBody)
        {
            std::string res = conn->parser->MakeRes(resBody);
            Super::Write(connId, MakeBuffer(res));
        };

        if (err->first)
        {
            // error
            if (err->first == 1)
            {
                // EOF
            }
            else
            {
                handleServerRequest(connId, err, req, cbkWrite);
            }
            return;
        }

        Error parseErr;
        {
            std::unique_lock writeLock(conn->connMutex);
            if (conn->recv == nullptr)
                conn->recv = buffer;
            else
                conn->recv->insert(conn->recv->end(), buffer->begin(), buffer->end());
            parseErr = conn->parser->ParseReq(conn->recv, req);
        }
        handleServerRequest(connId, parseErr, req, cbkWrite);
    }
};
