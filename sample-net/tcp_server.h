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

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable:4566)
#endif

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
    mutable Mutex stateMutex;

    HandleServerStates handleStates;
    HandleServerConnStates handleConnStates;
    HandleServerConnRead handleConnRead;

    std::map<ConnId, ConnectionStates> connStates;
    mutable Mutex connStatesMutex;

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
        RLock readLock(stateMutex);
        return state;
    }
    void SetState(ServerStates new_state, Error err = MakeSuccess())
    {
        {
            WLock writeLock(stateMutex);
            state = new_state;
        }
        if (handleStates)
            threadPool->MoveToThread([=]
                                     { handleStates(new_state, err); });
    }
    bool IsRunning() const { return CheckIsRunning(GetState()); }

    bool WaitForState(ServerStates state, double timeout_sec)
    {
        return WaitForState({state}, timeout_sec);
    }

    bool WaitForState(std::initializer_list<ServerStates> state_list, double timeout_sec)
    {
        std::set<ServerStates> states(state_list);
        auto start = std::chrono::steady_clock::now();
        std::chrono::milliseconds duration(int(timeout_sec * 1000));
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
        RLock readLock(connStatesMutex);
        auto ite = connStates.find(connId);
        if (ite != connStates.end())
            return ite->second;
        return ConnectionStates::Closed;
    }
    void SetConnState(ConnId connId, ConnectionStates new_state, Error err = MakeSuccess())
    {
        {
            WLock writeLock(connStatesMutex);
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
            RLock readLock(connStatesMutex);
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
    bool Listen()
    {
        if (IsRunning())
            return false;

        SetState(ServerStates::Listen);
        server->Listen(GetAddr(), GetPort());
        return true;
    }

    bool Close()
    {
        if (GetState() <= ServerStates::Closing)
            return false;

        SetState(ServerStates::Closing);
        server->Close();
        return true;
    }

    bool Write(ConnId connId, Buffer buffer)
    {
        if (!IsRunning())
            return false;

        server->Write(connId, buffer);
        return true;
    }

    bool Write(Buffer buffer)
    {
        if (!IsRunning())
            return false;

        std::vector<ConnId> connIds = GetConns(ConnectionStates::Connected);
        for (auto connId : connIds)
            server->Write(connId, buffer);
        return true;
    }

    /* 阻塞等待 */
    bool ListenSync(double timeout_sec = 5)
    {
        Listen();
        return WaitForState({ServerStates::Listening, ServerStates::ListenFailed}, timeout_sec);
    }

    bool CloseSync(double timeout_sec = 5)
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
        SetConnState(connId, ConnectionStates::Connected, err);
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
