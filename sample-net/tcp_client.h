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
using HandleClientStates = std::function<void(ConnectionState, Error)>;
using HandleClientRead = std::function<void(Buffer)>;

template <class ITcpClientImpl>
class TcpClient
{
protected:
    std::unique_ptr<IClient> client = std::make_unique<ITcpClientImpl>();

private:
    std::string addr;
    int port;
    std::string tips;

    ConnectionState state = ConnectionState::Closed;
    mutable Mutex stateMutex;
    HandleClientStates handleStates;
    HandleClientRead handleRead;

public:
    TcpClient(const std::string &addr = "127.0.0.1", int port = 49151, const std::string &tips = "TcpClient")
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

    ConnectionState GetState() const
    {
        RLock lock(stateMutex);
        return state;
    }
    void SetState(ConnectionState new_state, Error err = MakeSuccess())
    {
        {
            WLock lock(stateMutex);
            state = new_state;
        }
        if (handleStates)
            handleStates(new_state, err);
    }
    bool IsRunning() const { return CheckIsRunning(GetState()); }

    bool WaitForState(ConnectionState state, double timeout_sec)
    {
        return WaitForState({state}, timeout_sec) == state;
    }

    ConnectionState WaitForState(std::initializer_list<ConnectionState> state_list, double timeout_sec)
    {
        std::set<ConnectionState> states(state_list);
        auto start = std::chrono::steady_clock::now();
        std::chrono::milliseconds duration(int(timeout_sec * 1000));
        while (std::chrono::steady_clock::now() - start < duration)
        {
            ConnectionState state = GetState();
            if (states.find(state) != states.end())
                return state;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        return GetState();
    }

    /* 线程异步回调，注意线程安全 */
    void SetHandleStates(HandleClientStates f) { handleStates = f; }

    /* 线程异步回调，注意线程安全 */
    void SetHandleRead(HandleClientRead f) { handleRead = f; }

public:
    /* 异步，线程安全，可重入 */
    bool Connect()
    {
        if (IsRunning())
            return false;

        SetState(ConnectionState::Connecting);
        client->Connect(GetAddr(), GetPort());
        return true;
    }

    bool Close()
    {
        if (GetState() <= ConnectionState::Closing)
            return false;

        SetState(ConnectionState::Closing);
        client->Close();
        return true;
    }

    bool Write(Buffer buffer)
    {
        if (!IsRunning())
            return false;

        client->Write(buffer);
        return true;
    }

    /* 阻塞等待 */
    bool ConnectSync(double timeout_sec = 5)
    {
        Connect();
        return WaitForState({ConnectionState::Connected, ConnectionState::ConnFailed}, timeout_sec) == ConnectionState::Connected;
    }

    bool CloseSync(double timeout_sec = 5)
    {
        Close();
        return WaitForState(ConnectionState::Closed, timeout_sec);
    }

protected:
    /* 单线程同步，不可重入 */
    virtual void OnConnect(Error err)
    {
        if (err->first)
        {
            // err
            SetState(ConnectionState::ConnFailed, err);
            return;
        }
        // success
        SetState(ConnectionState::Connected);
    }

    virtual void OnClose(Error err)
    {
        SetState(ConnectionState::Closed, err);
    }

    virtual void OnWrite(Error err)
    {
        if (err->first)
        {
            // error
            SetState(ConnectionState::Shutdown, err);
            client->Close();
            return;
        }
    }

    virtual void OnRead(Error err, Buffer buffer)
    {
        if (err->first)
        {
            // error
            SetState(ConnectionState::Shutdown, err);
            client->Close();
            return;
        }

        // success
        if (handleRead)
            handleRead(buffer);
    }
};
