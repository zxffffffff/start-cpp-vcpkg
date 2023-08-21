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
using HandleClientStates = std::function<void(ConnectionStates)>;
using HandleClientRead = std::function<void(Buffer)>;

template <class ITcpClientImpl, class IThreadPoolImpl>
class TcpClient
{
    std::unique_ptr<IClient> client = std::make_unique<ITcpClientImpl>();
    std::unique_ptr<IThreadPool> threadPool = std::make_unique<IThreadPoolImpl>();

private:
    std::string addr;
    int port;
    std::string tips;

    std::atomic_bool retryConn = false;
    std::mutex retryMutex;

    ConnectionStates state = ConnectionStates::Closed;
    mutable std::shared_mutex stateMutex;
    HandleClientStates handleStates;
    HandleClientRead handleRead;

public:
    TcpClient(const std::string &addr = "127.0.0.1", int port = 12345, const std::string &tips = "TcpClient")
        : addr(addr), port(port), tips(tips)
    {
        using namespace std::placeholders;
        client->SetHandleRead(std::bind(&TcpClient::OnRead, this, _1, _2));
    }

    virtual ~TcpClient()
    {
        SetRetryConnect(false);
        if (IsRunning())
            CloseSync();
    }

    void SetAddr(const std::string &addr) { this->addr = addr; }
    const std::string &GetAddr() const { return addr; }

    void SetPort(int port) { this->port = port; }
    int GetPort() const { return port; }

    void SetRetryConnect(bool retry) { retryConn = retry; }

    ConnectionStates GetState() const
    {
        std::shared_lock lock(stateMutex);
        return state;
    }
    void SetState(ConnectionStates new_state)
    {
        {
            std::unique_lock lock(stateMutex);
            state = new_state;
        }
        if (handleStates)
            threadPool->MoveToThread([=] { handleStates(new_state); return true; });
    }
    bool IsRunning() const { return CheckIsRunning(GetState()); }

    /* 线程异步回调，注意线程安全 */
    void SetHandleStates(HandleClientStates f) { handleStates = f; }

    /* 线程异步回调，注意线程安全 */
    void SetHandleRead(HandleClientRead f) { handleRead = f; }

public:
    /* 异步，线程安全，可重入 */
    std::future<Error> Connect()
    {
        return threadPool->MoveToThread<Error>(std::bind(&TcpClient::ConnectSync, this));
    }

    std::future<Error> Close()
    {
        return threadPool->MoveToThread<Error>(std::bind(&TcpClient::CloseSync, this));
    }

    std::future<Error> ReConn()
    {
        return threadPool->MoveToThread<Error>(std::bind(&TcpClient::ReConnSync, this));
    }

    std::future<Error> Write(Buffer buffer)
    {
        return threadPool->MoveToThread<Error>([=] { return WriteSync(buffer); });
    }

    virtual void OnRead(Error err, Buffer buffer)
    {
        threadPool->MoveToThread([=] { return OnReadSync(err, buffer); });
    }

    /* 同步，线程安全，可重入 */
    Error ConnectSync()
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

        SetState(ConnectionStates::Connecting);
        auto future = client->Connect(GetAddr(), GetPort());
        auto err = future.get();
        if (err->first)
        {
            // err
            std::stringstream ss;
            ss << __func__
               << " addr=" << addr
               << " port=" << port
               << " tips=" << tips
               << " state=" << ToString(GetState())
               << " errCode=" << err->first
               << " errMsg" << err->second;

            SetState(ConnectionStates::ConnFailed);
            if (retryConn)
                ReConn();
            return MakeError(false, ss.str());
        }
        // success
        SetState(ConnectionStates::Connected);
        return MakeSuccess();
    }

    Error CloseSync()
    {
        if (GetState() <= ConnectionStates::Closing)
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

        SetState(ConnectionStates::Closing);
        auto future = client->Close();
        auto err = future.get();
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
        SetState(ConnectionStates::Closed);
        return MakeSuccess();
    }

    Error ReConnSync()
    {
        if (!retryMutex.try_lock())
            return MakeError(false, "!retryMutex.try_lock");

        /* 指数退避算法可以在每次重连失败后，将等待时间逐渐增加，以避免频繁的重连尝试 */
        std::function<int(int)> fibonacci = [&fibonacci](int n)
        {
            if (n <= 0)
                return 0;
            if (n == 1)
                return 1;
            return fibonacci(n - 1) + fibonacci(n - 2);
        };

        bool ok = false;
        int sleep_ms = 0;
        int reconnectCount = 0;
        for (; retryConn && reconnectCount < 9999; ++reconnectCount)
        {
            CloseSync();
            auto ret = ConnectSync();
            ok = ret->first;
            if (ok)
                break;

            if (sleep_ms < 60 * 1000)
            {
                sleep_ms = fibonacci(reconnectCount);
                sleep_ms = std::min(sleep_ms * 100, 60 * 1000);
            }
            if (sleep_ms)
            {
                using namespace std::chrono;
                using namespace std::chrono_literals;

                auto start = steady_clock::now();
                milliseconds duration(sleep_ms);
                while (retryConn && duration_cast<milliseconds>(steady_clock::now() - start).count() < duration.count())
                {
                    std::this_thread::sleep_for(10ms);
                }
            }
        }

        retryMutex.unlock();

        if (!ok)
        {
            // err
            std::stringstream ss;
            ss << __func__
               << " addr=" << addr
               << " port=" << port
               << " tips=" << tips
               << " state=" << ToString(GetState())
               << " reconnectCount=" << reconnectCount;
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

        auto future = client->Write(buffer);
        auto err = future.get();
        if (err->first)
        {
            // err
            std::stringstream ss;
            ss << __func__
               << " addr=" << addr
               << " port=" << port
               << " tips=" << tips
               << " state=" << ToString(GetState())
               << " bufferSize=" << buffer->size()
               << " errCode=" << err->first
               << " errMsg" << err->second;

            SetState(ConnectionStates::NetError);
            if (retryConn)
                ReConn();
            return MakeError(false, ss.str());
        }
        // success
        return MakeSuccess();
    }

    virtual Error OnReadSync(Error err, Buffer buffer)
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
               << " bufferSize=" << buffer->size()
               << " errCode=" << err->first
               << " errMsg" << err->second;

            SetState(ConnectionStates::NetError);
            if (retryConn)
                ReConn();
            return MakeError(false, ss.str());
        }

        // success
        if (handleRead)
            threadPool->MoveToThread([=] { handleRead(buffer); return true; });
        return MakeSuccess();
    }
};
