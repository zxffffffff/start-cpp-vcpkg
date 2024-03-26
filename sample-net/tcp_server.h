/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include "interface/tcp_interface.h"
#include "net_states.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable : 4566)
#endif

/* 线程异步回调，注意线程安全 */
using HandleServerState = std::function<void(ServerState, Error)>;
using HandleServerConnStates = std::function<void(ConnId, ConnectionState, Error)>;
using HandleServerConnRead = std::function<void(ConnId, Buffer)>;

template <class T_ITcpServerImpl>
class TcpServer
{
protected:
    std::unique_ptr<IServer> server = std::make_unique<T_ITcpServerImpl>();

private:
    std::string addr;
    int port;
    int maxConn;
    std::string tips;

    ServerState state = ServerState::Closed;
    mutable Mutex stateMutex;

    HandleServerState handleStates;
    HandleServerConnStates handleConnStates;
    HandleServerConnRead handleConnRead;

    std::map<ConnId, ConnectionState> connStates;
    mutable Mutex connStatesMutex;

public:
    TcpServer(const std::string &addr = "127.0.0.1", int port = 49151, int maxConn = 1024, const std::string &tips = "TcpServer")
        : addr(addr), port(port), maxConn(maxConn), tips(tips)
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

    ServerState GetState() const
    {
        RLock readLock(stateMutex);
        return state;
    }
    void SetState(ServerState new_state, Error err = MakeSuccess())
    {
        {
            WLock writeLock(stateMutex);
            state = new_state;
        }
        if (handleStates)
            handleStates(new_state, err);
    }
    bool IsRunning() const { return CheckIsRunning(GetState()); }

    bool WaitForState(ServerState state, double timeout_sec)
    {
        return WaitForState({state}, timeout_sec) == state;
    }

    ServerState WaitForState(std::initializer_list<ServerState> state_list, double timeout_sec)
    {
        std::set<ServerState> states(state_list);
        auto start = std::chrono::steady_clock::now();
        std::chrono::milliseconds duration(int(timeout_sec * 1000));
        while (std::chrono::steady_clock::now() - start < duration)
        {
            ServerState state = GetState();
            if (states.find(state) != states.end())
                return state;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        return GetState();
    }

    ConnectionState GetConnState(ConnId connId)
    {
        RLock readLock(connStatesMutex);
        auto ite = connStates.find(connId);
        if (ite != connStates.end())
            return ite->second;
        return ConnectionState::Closed;
    }
    void SetConnState(ConnId connId, ConnectionState new_state, Error err = MakeSuccess())
    {
        {
            WLock writeLock(connStatesMutex);
            if (new_state == ConnectionState::Closed)
                connStates.erase(connStates.find(connId));
            else
                connStates[connId] = new_state;
        }

        if (handleConnStates)
            handleConnStates(connId, new_state, err);
    }
    std::vector<ConnId> GetConns(ConnectionState find_state)
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
    void SetHandleStates(HandleServerState f) { handleStates = f; }

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

        SetState(ServerState::Listen);
        server->Listen(GetAddr(), GetPort(), maxConn);
        return true;
    }

    bool Close()
    {
        if (GetState() <= ServerState::Closing)
            return false;

        SetState(ServerState::Closing);
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

        std::vector<ConnId> connIds = GetConns(ConnectionState::Connected);
        for (auto connId : connIds)
            server->Write(connId, buffer);
        return true;
    }

    /* 阻塞等待 */
    bool ListenSync(double timeout_sec = 5)
    {
        Listen();
        return WaitForState({ServerState::Listening, ServerState::ListenFailed}, timeout_sec) == ServerState::Listening;
    }

    bool CloseSync(double timeout_sec = 5)
    {
        Close();
        return WaitForState(ServerState::Closed, timeout_sec);
    }

protected:
    /* 单线程同步，不可重入 */
    virtual void OnListen(Error err)
    {
        if (err->first)
        {
            // error
            SetState(ServerState::ListenFailed, err);
            return;
        }
        // success
        SetState(ServerState::Listening);
    }

    virtual void OnClose(Error err)
    {
        SetState(ServerState::Closed, err);
    }

    virtual void OnNewConn(ConnId connId, Error err)
    {
        SetConnState(connId, ConnectionState::Connected, err);
    }

    virtual void OnCloseConn(ConnId connId, Error err)
    {
        SetConnState(connId, ConnectionState::Closed, err);
    }

    virtual void OnConnWrite(ConnId connId, Error err)
    {
        if (err->first)
        {
            // error
            SetConnState(connId, ConnectionState::Shutdown, err);
            server->Close(connId);
            return;
        }
    }

    virtual void OnConnRead(ConnId connId, Error err, Buffer buffer)
    {
        if (err->first)
        {
            // error
            SetConnState(connId, ConnectionState::Shutdown, err);
            server->Close(connId);
            return;
        }

        // success
        if (handleConnRead)
            handleConnRead(connId, buffer);
    }
};
