/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include "tcp_interface.h"
#include "net_states.h"
#include "threadpool_interface.h"

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

    std::map<ConnId, ConnectionStates> connections;
    mutable std::shared_mutex connectionsMutex;

public:
    TcpServer(const std::string& addr = "127.0.0.1", int port = 12345, const std::string& tips = "TcpServer")
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
        std::shared_lock readLock(stateMutex);
        return state;
    }
    void SetState(ServerStates new_state)
    {
        {
            std::unique_lock writeLock(stateMutex);
            state = new_state;
        }
        if (handleStates)
            threadPool->MoveToThread([=] { handleStates(new_state); return true; });
    }
    bool IsRunning() const { return CheckIsRunning(GetState()); }

    ConnectionStates GetConnState(ConnId connId)
    {
        std::shared_lock readLock(connectionsMutex);
        auto ite = connections.find(connId);
        if (ite != connections.end())
            return ite->second;
        return ConnectionStates::Closed;
    }
    void SetConnState(ConnId connId, ConnectionStates new_state)
    {
        {
            std::unique_lock writeLock(connectionsMutex);
            if (new_state == ConnectionStates::Closed)
                connections.erase(connections.find(connId));
            else
                connections[connId] = new_state;
        }

        if (handleConnStates)
            threadPool->MoveToThread([=] { handleConnStates(connId, new_state); return true; });
    }
    std::vector<ConnId> GetConns(ConnectionStates find_state)
    {
        std::vector<ConnId> ret;
        {
            std::shared_lock readLock(connectionsMutex);
            for (auto ite = connections.begin(); ite != connections.end(); ++ite)
            {
                if (ite->second == find_state)
                    ret.push_back(ite->first);
            }
        }
        return ret;
    }

    /* 线程异步回调，注意线程安全 */
    void SetHandleStates(HandleServerStates f) { handleStates = f; }

    /* 线程异步回调，注意线程安全 */
    void SetHandleConnStates(HandleServerConnStates f) { handleConnStates = f; }

    /* 线程异步回调，注意线程安全 */
    void SetHandleConnRead(HandleServerConnRead f) { handleConnRead = f; }

public:
    /* 异步 */
    std::future<Error> Listen()
    {
        return threadPool->MoveToThread<Error>(std::bind(&TcpServer::ListenSync, this));
    }

    std::future<Error> Close()
    {
        return threadPool->MoveToThread<Error>(std::bind(&TcpServer::CloseSync, this));
    }

    std::future<Error> Write(ConnId connId, Buffer buffer)
    {
        return threadPool->MoveToThread<Error>([=] { return WriteSync(connId, buffer); });
    }

    std::future<Error> Write(Buffer buffer)
    {
        return threadPool->MoveToThread<Error>([=] { return WriteSync(buffer); });
    }

    virtual void OnNewConn(ConnId connId)
    {
        threadPool->MoveToThread([=] { OnNewConnSync(connId); });
    }

    virtual void OnCloseConn(ConnId connId)
    {
        threadPool->MoveToThread([=] { OnCloseConnSync(connId); });
    }

    virtual void OnConnRead(ConnId connId, Error err, Buffer buffer)
    {
        threadPool->MoveToThread([=] { OnConnReadSync(connId, err, buffer); });
    }

    /* 同步 */
    Error ListenSync()
    {
        if (IsRunning())
        {
            std::stringstream ss;
            ss << __func__
                << " addr=" << addr
                << " port=" << port
                << " tips=" << tips
                << " state=" << ToString(GetState())
                << " Is Running !";
            return MakeError(false, ss.str());
        }

        SetState(ServerStates::Listen);
        auto err = server->Listen(GetAddr(), GetPort()).get();
        if (err->first)
        {
            // err
            std::stringstream ss;
            ss << __func__
                << " addr=" << addr
                << " port=" << port
                << " tips=" << tips
                << " state=" << ToString(GetState())
                << " Listen Failed !";
            SetState(ServerStates::ListenFailed);
            return MakeError(false, ss.str());
        }
        // success
        SetState(ServerStates::Listening);
        return MakeSuccess();
    }

    Error CloseSync()
    {
        if (GetState() <= ServerStates::Closing)
        {
            std::stringstream ss;
            ss << __func__
                << " addr=" << addr
                << " port=" << port
                << " tips=" << tips
                << " state=" << ToString(GetState())
                << " Is Close/ing !";
            return MakeError(false, ss.str());
        }

        SetState(ServerStates::Closing);
        auto err = server->Close().get();
        if (err->first)
        {
            // never
            std::stringstream ss;
            ss << __func__
                << " addr=" << addr
                << " port=" << port
                << " tips=" << tips
                << " state=" << ToString(GetState())
                << " errCode=" << err->first
                << " errMsg" << err->second;

            assert(false);
            return MakeError(false, ss.str());
        }

        // success
        SetState(ServerStates::Closed);
        return MakeSuccess();
    }

    Error WriteSync(ConnId connId, Buffer buffer)
    {
        if (!IsRunning())
        {
            std::stringstream ss;
            ss << __func__
                << " addr=" << addr
                << " port=" << port
                << " tips=" << tips
                << " state=" << ToString(GetState())
                << " connId=" << connId
                << " bufferSize=" << buffer->size()
                << " Is Not Running !";
            return MakeError(false, ss.str());
        }

        auto err = server->Write(connId, buffer).get();
        if (err->first)
        {
            // err
            std::stringstream ss;
            ss << __func__
                << " addr=" << addr
                << " port=" << port
                << " tips=" << tips
                << " state=" << ToString(GetState())
                << " connId=" << connId
                << " bufferSize=" << buffer->size()
                << " errCode=" << err->first
                << " errMsg" << err->second;

            SetConnState(connId, ConnectionStates::NetError);
            return MakeError(false, ss.str());
        }

        // success
        return MakeSuccess();
    }

    Error WriteSync(Buffer buffer)
    {
        if (!IsRunning())
        {
            std::stringstream ss;
            ss << __func__
                << " addr=" << addr
                << " port=" << port
                << " tips=" << tips
                << " state=" << ToString(GetState())
                << " bufferSize=" << buffer->size()
                << " Is Not Running !";

            return MakeError(false, ss.str());
        }

        std::vector<ConnId> connIds = GetConns(ConnectionStates::Connected);
        std::vector<std::future<Error>> futures;
        for (auto connId : connIds)
        {
            futures.push_back(Write(connId, buffer));
        }
        int cnt = 0;
        for (auto& future : futures)
        {
            if (future.get())
                ++cnt;
        }

        if (cnt == 0)
        {
            std::stringstream ss;
            ss << __func__
                << " addr=" << addr
                << " port=" << port
                << " tips=" << tips
                << " state=" << ToString(GetState())
                << " bufferSize=" << buffer->size()
                << " percent=" << cnt << "/" << futures.size();
            return MakeError(false, ss.str());
        }
        return MakeSuccess();
    }

    virtual void OnNewConnSync(ConnId connId)
    {
        SetConnState(connId, ConnectionStates::Connected);
    }

    virtual void OnCloseConnSync(ConnId connId)
    {
        SetConnState(connId, ConnectionStates::Closed);
    }

    virtual Error OnConnReadSync(ConnId connId, Error err, Buffer buffer)
    {
        if (err->first)
        {
            // err
            std::stringstream ss;
            ss << __func__
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
            return MakeError(false, ss.str());
        }

        // success
        if (handleConnRead)
            threadPool->MoveToThread([=] { handleConnRead(connId, buffer); return true; });
        return MakeSuccess();
    }
};
