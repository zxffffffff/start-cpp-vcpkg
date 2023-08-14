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

/* 兼容 windows */
#undef min
#undef max

using namespace std::chrono_literals;

/* 线程异步回调，注意线程安全 */
using HandleClientStates = std::function<void(ConnectionStates)>;
using HandleClientRead = std::function<void(Buffer)>;

template<class ITcpClientImpl, class IThreadPoolImpl>
class TcpClient
{
    std::string addr;
    int port;
    std::string tips;

    std::atomic_bool retryConn = false;
    std::mutex retryMutex;

    ConnectionStates state = ConnectionStates::Closed;
    mutable std::shared_mutex stateMutex;
    HandleClientStates handleStates;
    HandleClientRead handleRead;

    std::unique_ptr<IClient> client = std::make_unique<ITcpClientImpl>();
    std::unique_ptr<IThreadPool> threadPool = std::make_unique<IThreadPoolImpl>();

public:
    TcpClient(const std::string& addr = "127.0.0.1", int port = 12345, const std::string& tips = "client")
        : addr(addr)
        , port(port)
        , tips(tips)
    {
        using namespace std::placeholders;
        client->SetHandleRead(std::bind(&TcpClient::OnRead, this, _1, _2));
    }

    virtual ~TcpClient()
    {
        if (IsRunning())
            CloseSync();
    }

    void SetAddr(const std::string& addr) { this->addr = addr; }
    const std::string& GetAddr() const { return addr; }

    void SetPort(int port) { this->port = port; }
    int GetPort() const { return port; }

    void SetRetryConnect(bool retry) { retryConn = retry; }

    ConnectionStates GetState() const
    {
        std::shared_lock lock(stateMutex);
        return state;
    }
    void SetState(ConnectionStates new_state, bool call = true)
    {
        {
            std::unique_lock lock(stateMutex);
            state = new_state;
        }
        if (call && handleStates)
            threadPool->MoveToThread([=] { handleStates(new_state); return true; });
    }
    bool IsRunning() const { return CheckIsRunning(GetState()); }

    /* 线程异步回调，注意线程安全 */
    void SetHandleStates(HandleClientStates f) { handleStates = f; }

    /* 线程异步回调，注意线程安全 */
    void SetHandleRead(HandleClientRead f) { handleRead = f; }

public:
    /* 异步 */
    std::future<bool> Connect()
    {
        return threadPool->MoveToThread(std::bind(&TcpClient::ConnectSync, this));
    }

    std::future<bool> Close()
    {
        return threadPool->MoveToThread(std::bind(&TcpClient::CloseSync, this));
    }

    std::future<bool> ReConn()
    {
        return threadPool->MoveToThread(std::bind(&TcpClient::ReConnSync, this));
    }

    std::future<bool> Write(Buffer buffer)
    {
        return threadPool->MoveToThread([=] { return WriteSync(buffer); });
    }

    void OnRead(Error err, Buffer buffer)
    {
        threadPool->MoveToThread([=] { return OnReadSync(err, buffer); });
    }

    /* 同步 */
    bool ConnectSync()
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

        SetState(ConnectionStates::Connecting);
        auto future = client->Connect(GetAddr(), GetPort());
        auto err = future.get();
        if (err->first)
        {
            // err
            LOG(ERROR) << __func__
                << " addr=" << addr
                << " port=" << port
                << " tips=" << tips
                << " state=" << ToString(GetState())
                << " errCode=" << err->first
                << " errMsg" << err->second;

            SetState(ConnectionStates::ConnFailed);
            if (retryConn)
                ReConn();
            return false;
        }

        // success
        LOG(INFO) << __func__
            << " addr=" << addr
            << " port=" << port
            << " tips=" << tips
            << " state=" << ToString(GetState());

        SetState(ConnectionStates::Connected);
        return true;
    }

    bool CloseSync()
    {
        if (GetState() <= ConnectionStates::Closing)
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

        SetState(ConnectionStates::Closing);
        auto future = client->Close();
        auto err = future.get();
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

        SetState(ConnectionStates::Closed);
        return true;
    }

    bool ReConnSync()
    {
        if (!retryMutex.try_lock())
            return false;

        /* 指数退避算法可以在每次重连失败后，将等待时间逐渐增加，以避免频繁的重连尝试 */
        std::function<int(int)> fibonacci = [&fibonacci](int n)
        {
            if (n <= 0) return 0;
            if (n == 1) return 1;
            return fibonacci(n - 1) + fibonacci(n - 2);
        };

        bool ok = false;
        for (int reconnectCount = 0; retryConn && reconnectCount < 9999; ++reconnectCount)
        {
            LOG(WARNING) << __func__
                << " addr=" << addr
                << " port=" << port
                << " tips=" << tips
                << " state=" << ToString(GetState())
                << " reconnectCount=" << reconnectCount;

            CloseSync();
            ok = ConnectSync();
            if (ok)
                break;

            if (int sleep_ms = fibonacci(reconnectCount))
            {
                std::this_thread::sleep_for(1ms * std::min(sleep_ms * 100, 60 * 1000));
            }
        }
        LOG(WARNING) << __func__
            << " addr=" << addr
            << " port=" << port
            << " tips=" << tips
            << " state=" << ToString(GetState())
            << " ok=" << ok;

        retryMutex.unlock();
        return ok;
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

        auto future = client->Write(buffer);
        auto err = future.get();
        if (err->first)
        {
            // err
            LOG(ERROR) << __func__
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
            return false;
        }

        // success
        LOG(INFO) << __func__
            << " addr=" << addr
            << " port=" << port
            << " tips=" << tips
            << " state=" << ToString(GetState())
            << " bufferSize=" << buffer->size();
        return true;
    }

    bool OnReadSync(Error err, Buffer buffer)
    {
        if (err->first)
        {
            // err
            LOG(ERROR) << __func__
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
            return false;
        }

        // success
        LOG(INFO) << __func__
            << " addr=" << addr
            << " port=" << port
            << " tips=" << tips
            << " state=" << ToString(GetState())
            << " bufferSize=" << buffer->size();

        if (handleRead)
            threadPool->MoveToThread([=] { handleRead(buffer); return true; });
        return true;
    }
};
