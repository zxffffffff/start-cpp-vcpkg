/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include "interface/tcp_interface.h"
#include "interface/threadpool_interface.h"
#include "net_states.h"

/* 线程异步回调，注意线程安全 */
using HandleServerStates = std::function<void(ServerStates, Error)>;
using HandleServerConnStates = std::function<void(ConnId, ConnectionStates, Error)>;
using HandleServerConnRead = std::function<void(ConnId, Buffer)>;

template <class ITcpServerImpl, class IThreadPoolImpl>
class TcpServer
{
protected:
    std::unique_ptr<IServer> server = std::make_unique<ITcpServerImpl>();
    std::unique_ptr<IThreadPool> threadPool = std::make_unique<IThreadPoolImpl>();

private:
    std::string addr;
    int port;
    std::string tips;

    ServerStates state = ServerStates::Closed;
    mutable std::shared_mutex stateMutex;

    HandleServerStates handleStates;
    HandleServerConnStates handleConnStates;
    HandleServerConnRead handleConnRead;

    std::map<ConnId, ConnectionStates> connStates;
    mutable std::shared_mutex connStatesMutex;

public:
    TcpServer(const std::string &addr = "127.0.0.1", int port = 12345, const std::string &tips = "TcpServer")
        : addr(addr), port(port), tips(tips)
    {
        using namespace std::placeholders;
        server->SetHandleListen(std::bind(&TcpServer::OnListen, this, _1));
        server->SetHandleClose(std::bind(&TcpServer::OnClose, this, _1));
        server->SetHandleNewConn(std::bind(&TcpServer::OnNewConn, this, _1, _2));
        server->SetHandleCloseConn(std::bind(&TcpServer::OnCloseConn, this, _1, _2));
        server->SetHandleConnWrite(std::bind(&TcpServer::OnConnWrite, this, _1, _2));
        server->SetHandleConnRead(std::bind(&TcpServer::OnConnRead, this, _1, _2, _3));
    }

    virtual ~TcpServer()
    {
        if (IsRunning())
            CloseSync();
    }

    void SetAddr(const std::string &addr) { this->addr = addr; }
    const std::string &GetAddr() const { return addr; }

    void SetPort(int port) { this->port = port; }
    int GetPort() const { return port; }

    ServerStates GetState() const
    {
        std::shared_lock readLock(stateMutex);
        return state;
    }
    void SetState(ServerStates new_state, Error err = MakeSuccess())
    {
        {
            std::unique_lock writeLock(stateMutex);
            state = new_state;
        }
        if (handleStates)
            threadPool->MoveToThread([=]
                                     { handleStates(new_state, err); });
    }
    bool IsRunning() const { return CheckIsRunning(GetState()); }

    bool WaitForState(ServerStates state, int timeout_sec)
    {
        return WaitForState({state}, timeout_sec);
    }

    bool WaitForState(std::initializer_list<ServerStates> state_list, int timeout_sec)
    {
        std::set<ServerStates> states(state_list);
        auto start = std::chrono::steady_clock::now();
        std::chrono::milliseconds duration(timeout_sec * 1000);
        while (std::chrono::steady_clock::now() - start < duration)
        {
            if (states.find(GetState()) != states.end())
                return true;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        return (states.find(GetState()) != states.end());
    }

    ConnectionStates GetConnState(ConnId connId)
    {
        std::shared_lock readLock(connStatesMutex);
        auto ite = connStates.find(connId);
        if (ite != connStates.end())
            return ite->second;
        return ConnectionStates::Closed;
    }
    void SetConnState(ConnId connId, ConnectionStates new_state, Error err = MakeSuccess())
    {
        {
            std::unique_lock writeLock(connStatesMutex);
            if (new_state == ConnectionStates::Closed)
                connStates.erase(connStates.find(connId));
            else
                connStates[connId] = new_state;
        }

        if (handleConnStates)
            threadPool->MoveToThread([=]
                                     { handleConnStates(connId, new_state, err); });
    }
    std::vector<ConnId> GetConns(ConnectionStates find_state)
    {
        std::vector<ConnId> ret;
        {
            std::shared_lock readLock(connStatesMutex);
            for (auto ite = connStates.begin(); ite != connStates.end(); ++ite)
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
    /* 异步，线程安全，可重入 */
    void Listen()
    {
        if (IsRunning())
            return;

        SetState(ServerStates::Listen);
        server->Listen(GetAddr(), GetPort());
    }

    void Close()
    {
        if (GetState() <= ServerStates::Closing)
            return;

        SetState(ServerStates::Closing);
        server->Close();
    }

    void Write(ConnId connId, Buffer buffer)
    {
        if (!IsRunning())
            return;

        server->Write(connId, buffer);
    }

    void Write(Buffer buffer)
    {
        if (!IsRunning())
            return;

        std::vector<ConnId> connIds = GetConns(ConnectionStates::Connected);
        for (auto connId : connIds)
            server->Write(connId, buffer);
    }

    /* 阻塞等待 */
    bool ListenSync(int timeout_sec = 5)
    {
        Listen();
        return WaitForState({ServerStates::Listening, ServerStates::ListenFailed}, timeout_sec);
    }

    bool CloseSync(int timeout_sec = 5)
    {
        Close();
        return WaitForState(ServerStates::Closed, timeout_sec);
    }

protected:
    /* 单线程同步，不可重入 */
    virtual void OnListen(Error err)
    {
        if (err->first)
        {
            // error
            SetState(ServerStates::ListenFailed, err);
            return;
        }
        // success
        SetState(ServerStates::Listening);
    }

    virtual void OnClose(Error err)
    {
        SetState(ServerStates::Closed, err);
    }

    virtual void OnNewConn(ConnId connId, Error err)
    {
        if (err->first)
        {
            // error
            return;
        }
        SetConnState(connId, ConnectionStates::Connected);
    }

    virtual void OnCloseConn(ConnId connId, Error err)
    {
        SetConnState(connId, ConnectionStates::Closed, err);
    }

    virtual void OnConnWrite(ConnId connId, Error err)
    {
        if (err->first)
        {
            // error
            SetConnState(connId, ConnectionStates::Shutdown, err);
            server->Close(connId);
            return;
        }
    }

    virtual void OnConnRead(ConnId connId, Error err, Buffer buffer)
    {
        if (err->first)
        {
            // error
            SetConnState(connId, ConnectionStates::Shutdown, err);
            server->Close(connId);
            return;
        }

        // success
        if (handleConnRead)
            threadPool->MoveToThread([=]
                                     { handleConnRead(connId, buffer); });
    }
};
