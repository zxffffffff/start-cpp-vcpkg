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

/* 兼容 windows */
#undef min
#undef max

/* 线程异步回调，注意线程安全 */
using HandleClientStates = std::function<void(ConnectionStates, Error)>;
using HandleClientRead = std::function<void(Buffer)>;

template <class ITcpClientImpl, class IThreadPoolImpl>
class TcpClient
{
protected:
    std::unique_ptr<IClient> client = std::make_unique<ITcpClientImpl>();
    std::unique_ptr<IThreadPool> threadPool = std::make_unique<IThreadPoolImpl>();

private:
    std::string addr;
    int port;
    std::string tips;

    ConnectionStates state = ConnectionStates::Closed;
    mutable std::shared_mutex stateMutex;
    HandleClientStates handleStates;
    HandleClientRead handleRead;

public:
    TcpClient(const std::string &addr = "127.0.0.1", int port = 12345, const std::string &tips = "TcpClient")
        : addr(addr), port(port), tips(tips)
    {
        using namespace std::placeholders;
        client->SetHandleConnect(std::bind(&TcpClient::OnConnect, this, _1));
        client->SetHandleClose(std::bind(&TcpClient::OnClose, this, _1));
        client->SetHandleWrite(std::bind(&TcpClient::OnWrite, this, _1));
        client->SetHandleRead(std::bind(&TcpClient::OnRead, this, _1, _2));
    }

    virtual ~TcpClient()
    {
        if (IsRunning())
            CloseSync();
    }

    void SetAddr(const std::string &addr) { this->addr = addr; }
    const std::string &GetAddr() const { return addr; }

    void SetPort(int port) { this->port = port; }
    int GetPort() const { return port; }

    ConnectionStates GetState() const
    {
        std::shared_lock lock(stateMutex);
        return state;
    }
    void SetState(ConnectionStates new_state, Error err = MakeSuccess())
    {
        {
            std::unique_lock lock(stateMutex);
            state = new_state;
        }
        if (handleStates)
            threadPool->MoveToThread([=]
                                     { handleStates(new_state, err); });
    }
    bool IsRunning() const { return CheckIsRunning(GetState()); }

    bool WaitForState(ConnectionStates state, int timeout_sec)
    {
        return WaitForState({state}, timeout_sec);
    }

    bool WaitForState(std::initializer_list<ConnectionStates> state_list, int timeout_sec)
    {
        std::set<ConnectionStates> states(state_list);
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

    /* 线程异步回调，注意线程安全 */
    void SetHandleStates(HandleClientStates f) { handleStates = f; }

    /* 线程异步回调，注意线程安全 */
    void SetHandleRead(HandleClientRead f) { handleRead = f; }

public:
    /* 异步，线程安全，可重入 */
    void Connect()
    {
        if (IsRunning())
            return;

        SetState(ConnectionStates::Connecting);
        client->Connect(GetAddr(), GetPort());
    }

    void Close()
    {
        if (GetState() <= ConnectionStates::Closing)
            return;

        SetState(ConnectionStates::Closing);
        client->Close();
    }

    void Write(Buffer buffer)
    {
        if (!IsRunning())
            return;

        client->Write(buffer);
    }

    /* 阻塞等待 */
    bool ConnectSync(int timeout_sec = 5)
    {
        Connect();
        return WaitForState({ConnectionStates::Connected, ConnectionStates::ConnFailed}, timeout_sec);
    }

    bool CloseSync(int timeout_sec = 5)
    {
        Close();
        return WaitForState(ConnectionStates::Closed, timeout_sec);
    }

protected:
    /* 单线程同步，不可重入 */
    virtual void OnConnect(Error err)
    {
        if (err->first)
        {
            // err
            SetState(ConnectionStates::ConnFailed, err);
            return;
        }
        // success
        SetState(ConnectionStates::Connected);
    }

    virtual void OnClose(Error err)
    {
        SetState(ConnectionStates::Closed, err);
    }

    virtual void OnWrite(Error err)
    {
        if (err->first)
        {
            // error
            SetState(ConnectionStates::Shutdown, err);
            client->Close();
            return;
        }
    }

    virtual void OnRead(Error err, Buffer buffer)
    {
        if (err->first)
        {
            // error
            SetState(ConnectionStates::Shutdown, err);
            client->Close();
            return;
        }

        // success
        if (handleRead)
            threadPool->MoveToThread([=]
                                     { handleRead(buffer); });
    }
};
