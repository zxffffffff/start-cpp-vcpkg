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

/* 线程异步回调，注意线程安全 */
using ServerResponseCbk = std::function<void(std::string)>;
using HandleServerRequest = std::function<void(ConnId, std::shared_ptr<const HttpRequest>, ServerResponseCbk)>;

template <class IHttpParserImpl, class ITcpServerImpl, class IThreadPoolImpl>
class HttpServer : public TcpServer<ITcpServerImpl, IThreadPoolImpl>
{
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
    mutable std::shared_mutex connsMutex;

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
    /* 同步，线程安全，可重入 */
    virtual void OnNewConnSync(ConnId connId) override
    {
        Super::OnNewConnSync(connId);

        {
            std::shared_lock readLock(connsMutex);
            conns[connId] = std::make_shared<Connection>();
        }
    }

    virtual void OnCloseConnSync(ConnId connId) override
    {
        {
            std::unique_lock writeLock(connsMutex);
            conns.erase(conns.find(connId));
        }

        Super::OnCloseConnSync(connId);
    }

    virtual Error OnConnReadSync(ConnId connId, Error err, Buffer buffer) override
    {
        if (handleServerRequest && !err->first)
        {
            std::shared_ptr<Connection> conn;
            {
                std::shared_lock readLock(connsMutex);
                conn = conns[connId];
            }
            {
                std::unique_lock writeLock(conn->connMutex);
                if (conn->recv == nullptr)
                    conn->recv = buffer;
                else
                    conn->recv->insert(conn->recv->end(), buffer->begin(), buffer->end());
            }
            auto req = std::make_shared<HttpRequest>();
            auto parseErr = conn->parser->ParseReq(buffer, req);
            if (!parseErr->first)
            {
                auto cbk = [=](std::string resBody)
                {
                    std::string res = conn->parser->MakeRes(resBody);
                    Super::Write(connId, MakeBuffer(res));
                };
                handleServerRequest(connId, req, cbk);
            }
        }
        return Super::OnConnReadSync(connId, err, buffer);
    }
};
