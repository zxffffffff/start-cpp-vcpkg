/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include <gflags/gflags.h>
#include <glog/logging.h>
#include "tcp_proto.h"
#include "tcp_interface.h"
#include "thread/threadpool_interface.h"

/* 线程异步回调，注意线程安全 */
using HandleServerStates = std::function<void(ServerStates)>;
using HandleServerConnStates = std::function<void(ConnId, ConnectionStates)>;
using HandleServerConnRead = std::function<void(ConnId, Buffer)>;

template<class ITcpServerImpl, class IThreadPoolImpl>
class TcpServer
{
    std::string addr;
    int port;
    std::string tips;

    ServerStates state = ServerStates::Closed;
    mutable std::shared_mutex stateMutex;
    HandleServerStates handleStates;
    HandleServerConnStates handleConnStates;
    HandleServerConnRead handleConnRead;

    std::unique_ptr<IServer> server = std::make_unique<ITcpServerImpl>();
    std::unique_ptr<IThreadPool> threadPool = std::make_unique<IThreadPoolImpl>();

    struct Connection
    {
        ConnId connId = 0;
        ConnectionStates state = ConnectionStates::Closed;
        mutable std::shared_mutex stateMutex;
    };
    std::map<ConnId, Connection> connections;

public:
    TcpServer(const std::string& addr = "127.0.0.1", int port = 12345, const std::string& tips = "server")
        : addr(addr)
        , port(port)
        , tips(tips)
    {
        using namespace std::placeholders;
        server->SetHandleNewConn(std::bind(&TcpServer::OnNewConn, this, _1));
        server->SetHandleCloseConn(std::bind(&TcpServer::OnCloseConn, this, _1));
        server->SetHandleConnRead(std::bind(&TcpServer::OnConnRead, this, _1, _2, _3));
    }

    virtual ~TcpServer()
    {
        if (IsRunning())
            Close().get();
    }

    void SetAddr(const std::string& addr) { this->addr = addr; }
    const std::string& GetAddr() const { return addr; }

    void SetPort(int port) { this->port = port; }
    int GetPort() const { return port; }

    ServerStates GetState() const
    {
        std::shared_lock lock(stateMutex);
        return state;
    }
    void SetState(ServerStates new_state, bool call = true)
    {
        {
            std::unique_lock lock(stateMutex);
            state = new_state;
        }
        if (call && handleStates)
            threadPool->MoveToThread([=] { handleStates(new_state); return true; });
    }
    bool IsRunning() const { return CheckIsRunning(GetState()); }

    ConnectionStates GetConnState(ConnId connId)
    {
        auto ite = connections.find(connId);
        if (ite == connections.end())
        {
            LOG(ERROR) << __func__
                << " addr=" << addr
                << " port=" << port
                << " state=" << ToString(GetState())
                << " connId=" << connId
                << " no found !";
            return ConnectionStates::Closed;
        }
        Connection& conn = ite->second;
        std::shared_lock lock(conn.stateMutex);
        return conn.state;
    }
    void SetConnState(ConnId connId, ConnectionStates new_state, bool call = true)
    {
        Connection& conn = connections[connId];
        {
            std::unique_lock lock(conn.stateMutex);
            conn.state = new_state;
        }
        if (call && handleConnStates)
            threadPool->MoveToThread([=] { handleConnStates(connId, new_state); return true; });
    }

    /* 线程异步回调，注意线程安全 */
    void SetHandleStates(HandleServerStates f) { handleStates = f; }

    /* 线程异步回调，注意线程安全 */
    void SetHandleConnStates(HandleServerConnStates f) { handleConnStates = f; }

    /* 线程异步回调，注意线程安全 */
    void SetHandleConnRead(HandleServerConnRead f) { handleConnRead = f; }

public:
    /* 异步 */
    std::future<bool> Listen()
    {
        return threadPool->MoveToThread(std::bind(&TcpServer::ListenSync, this));
    }

    std::future<bool> Close()
    {
        return threadPool->MoveToThread(std::bind(&TcpServer::CloseSync, this));
    }

    std::future<bool> Write(ConnId connId, Buffer buffer)
    {
        return threadPool->MoveToThread([=] { return WriteSync(connId, buffer); });
    }

    std::future<bool> Write(Buffer buffer)
    {
        return threadPool->MoveToThread([=] { return WriteSync(buffer); });
    }

    void OnNewConn(ConnId connId)
    {
        threadPool->MoveToThread([=] { OnNewConnSync(connId); return true; });
    }

    void OnCloseConn(ConnId connId)
    {
        threadPool->MoveToThread([=] { OnCloseConnSync(connId); return true; });
    }

    void OnConnRead(ConnId connId, Error err, Buffer buffer)
    {
        threadPool->MoveToThread([=] { return OnConnReadSync(connId, err, buffer); });
    }

    /* 同步 */
    bool ListenSync()
    {
        if (IsRunning())
        {
            LOG(WARNING) << __func__
                << " addr=" << addr
                << " port=" << port
                << " tips=" << tips
                << " state=" << ToString(GetState())
                << " Is Running !";
            return false;
        }

        LOG(INFO) << __func__
            << " addr=" << addr
            << " port=" << port
            << " tips=" << tips
            << " state=" << ToString(GetState());

        SetState(ServerStates::Listen);
        auto err = server->Listen(GetAddr(), GetPort()).get();
        if (err->first)
        {
            // err
            LOG(ERROR) << __func__
                << " addr=" << addr
                << " port=" << port
                << " tips=" << tips
                << " state=" << ToString(GetState())
                << " Listen Failed !";
            SetState(ServerStates::ListenFailed);
            return false;
        }

        // success
        LOG(INFO) << __func__
            << " addr=" << addr
            << " port=" << port
            << " tips=" << tips
            << " state=" << ToString(GetState());
        SetState(ServerStates::Listening);
        return true;
    }

    bool CloseSync()
    {
        if (GetState() <= ServerStates::Closing)
        {
            LOG(WARNING) << __func__
                << " addr=" << addr
                << " port=" << port
                << " tips=" << tips
                << " state=" << ToString(GetState())
                << " Is Close/ing !";
            return false;
        }

        LOG(INFO) << __func__
            << " addr=" << addr
            << " port=" << port
            << " tips=" << tips
            << " state=" << ToString(GetState());

        SetState(ServerStates::Closing);
        auto err = server->Close().get();
        if (err->first)
        {
            // never
            LOG(ERROR) << __func__
                << " addr=" << addr
                << " port=" << port
                << " tips=" << tips
                << " state=" << ToString(GetState())
                << " errCode=" << err->first
                << " errMsg" << err->second;

            assert(false);
            return false;
        }

        // success
        LOG(INFO) << __func__
            << " addr=" << addr
            << " port=" << port
            << " tips=" << tips
            << " state=" << ToString(GetState());

        SetState(ServerStates::Closed);
        return true;
    }

    bool WriteSync(ConnId connId, Buffer buffer)
    {
        if (!IsRunning())
        {
            LOG(WARNING) << __func__
                << " addr=" << addr
                << " port=" << port
                << " tips=" << tips
                << " state=" << ToString(GetState())
                << " connId=" << connId
                << " bufferSize=" << buffer->size()
                << " Is Not Running !";
            return false;
        }

        LOG(INFO) << __func__
            << " addr=" << addr
            << " port=" << port
            << " tips=" << tips
            << " state=" << ToString(GetState())
            << " connId=" << connId
            << " bufferSize=" << buffer->size();

        auto& connection = connections[connId];
        auto err = server->Write(connId, buffer).get();
        if (err->first)
        {
            // err
            LOG(ERROR) << __func__
                << " addr=" << addr
                << " port=" << port
                << " tips=" << tips
                << " state=" << ToString(GetState())
                << " connId=" << connId
                << " bufferSize=" << buffer->size()
                << " errCode=" << err->first
                << " errMsg" << err->second;

            SetConnState(connId, ConnectionStates::NetError);
            return false;
        }

        // success
        LOG(INFO) << __func__
            << " addr=" << addr
            << " port=" << port
            << " tips=" << tips
            << " state=" << ToString(GetState())
            << " connId=" << connId
            << " bufferSize=" << buffer->size();
        return true;
    }

    bool WriteSync(Buffer buffer)
    {
        if (!IsRunning())
        {
            LOG(WARNING) << __func__
                << " addr=" << addr
                << " port=" << port
                << " tips=" << tips
                << " state=" << ToString(GetState())
                << " bufferSize=" << buffer->size()
                << " Is Not Running !";
            return false;
        }

        LOG(INFO) << __func__
            << " addr=" << addr
            << " port=" << port
            << " tips=" << tips
            << " state=" << ToString(GetState())
            << " bufferSize=" << buffer->size();

        std::vector<std::future<bool>> futures;
        for (auto ite = connections.begin(); ite != connections.end(); ++ite)
        {
            ConnId connId = ite->first;
            futures.push_back(Write(connId, buffer));
        }
        int ok = 0;
        for (auto& future : futures)
        {
            if (future.get())
                ++ok;
        }

        LOG(INFO) << __func__
            << " addr=" << addr
            << " port=" << port
            << " tips=" << tips
            << " state=" << ToString(GetState())
            << " bufferSize=" << buffer->size()
            << " ok=" << ok << "/" << futures.size();
        return ok;
    }

    void OnNewConnSync(ConnId connId)
    {
        LOG(INFO) << __func__
            << " addr=" << addr
            << " port=" << port
            << " tips=" << tips
            << " state=" << ToString(GetState())
            << " connId=" << connId;

        SetConnState(connId, ConnectionStates::Connected);
    }

    void OnCloseConnSync(ConnId connId)
    {
        LOG(INFO) << __func__
            << " addr=" << addr
            << " port=" << port
            << " tips=" << tips
            << " state=" << ToString(GetState())
            << " connState=" << ToString(GetConnState(connId))
            << " connId=" << connId;

        SetConnState(connId, ConnectionStates::Closed);
    }

    bool OnConnReadSync(ConnId connId, Error err, Buffer buffer)
    {
        if (err->first)
        {
            // err
            LOG(ERROR) << __func__
                << " addr=" << addr
                << " port=" << port
                << " tips=" << tips
                << " state=" << ToString(GetState())
                << " connState=" << ToString(GetConnState(connId))
                << " connId=" << connId
                << " bufferSize=" << buffer->size()
                << " errCode=" << err->first
                << " errMsg" << err->second;

            SetConnState(connId, ConnectionStates::NetError);
            return false;
        }

        // success
        LOG(INFO) << __func__
            << " addr=" << addr
            << " port=" << port
            << " tips=" << tips
            << " state=" << ToString(GetState())
            << " connState=" << ToString(GetConnState(connId))
            << " connId=" << connId
            << " bufferSize=" << buffer->size();

        if (handleConnRead)
            threadPool->MoveToThread([=] { handleConnRead(connId, buffer); return true; });
        return true;
    }
};
