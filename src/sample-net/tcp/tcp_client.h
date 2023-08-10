/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include "tcp_proto.h"
#include "tcp_interface.h"
#include "thread/threadpool_interface.h"

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
            Close().get();
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
            handleStates(new_state);
    }
    bool IsRunning() const { return CheckIsRunning(GetState()); }

    /* 线程异步回调，注意线程安全 */
    void SetHandleStates(HandleClientStates f) { handleStates = f; }

    /* 线程异步回调，注意线程安全 */
    void SetHandleRead(HandleClientRead f) { handleRead = f; }

public:
    std::future<bool> Connect()
    {
        if (IsRunning())
        {
            LOG(WARNING) << __func__ 
                << " addr=" << addr
                << " port=" << port 
                << " tips=" << tips
                << " state=" << ToString(GetState())
                << " Is Running !";
            return MakeFuture(false);
        }
        LOG(INFO) << __func__ 
            << " addr=" << addr
            << " port=" << port
            << " tips=" << tips
            << " state=" << ToString(GetState());

        SetState(ConnectionStates::Connecting);
        auto cbk = [this]
        {
            auto future = client->Connect(GetAddr(), GetPort());
            auto err = future.get();
            if (err->first)
            {
                // err
                LOG(ERROR) << "Connect-cbk"
                    << " addr=" << addr 
                    << " port=" << port 
                    << " tips=" << tips
                    << " state=" << ToString(GetState())
                    << " errCode=" << err->second
                    << " errMsg" << err->second;
                SetState(ConnectionStates::ConnFailed);
                if (retryConn)
                    ReConn();
                return false;
            }
            // success
            LOG(INFO) << "Connect-cbk"
                << " addr=" << addr 
                << " port=" << port 
                << " tips=" << tips
                << " state=" << ToString(GetState());
            SetState(ConnectionStates::Connected);
            return true;
        };
        return threadPool->MoveToThread(cbk);
    }

    std::future<bool> Close()
    {
        if (GetState() <= ConnectionStates::Closing)
        {
            LOG(WARNING) << __func__ 
                << " addr=" << addr 
                << " port=" << port
                << " tips=" << tips
                << " state=" << ToString(GetState())
                << " Is Close/ing !";
            return MakeFuture(false);
        }
        LOG(INFO) << __func__
            << " addr=" << addr 
            << " port=" << port 
            << " tips=" << tips
            << " state=" << ToString(GetState());

        SetState(ConnectionStates::Closing);
        auto cbk = [this]
        {
            auto future = client->Close();
            auto err = future.get();
            if (err->first)
            {
                // never
                LOG(ERROR) << "Close-cbk"
                    << " addr=" << addr 
                    << " port=" << port 
                    << " tips=" << tips
                    << " state=" << ToString(GetState())
                    << " errCode=" << err->second
                    << " errMsg" << err->second;
                assert(false);
                return false;
            }
            LOG(INFO) << "Close-cbk" 
                << " addr=" << addr 
                << " port=" << port 
                << " tips=" << tips
                << " state=" << ToString(GetState());
            SetState(ConnectionStates::Closed);
            return true;
        };
        return threadPool->MoveToThread(cbk);
    }

    std::future<bool> ReConn()
    {
        auto cbk = [this]
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
                LOG(WARNING) << "ReConn-cbk"
                    << " addr=" << addr
                    << " port=" << port
                    << " tips=" << tips
                    << " state=" << ToString(GetState())
                    << " reconnectCount=" << reconnectCount;

                Close().get();
                ok = Connect().get();
                if (ok)
                    break;

                if (int sleep_ms = fibonacci(reconnectCount))
                {
                    std::this_thread::sleep_for(1ms * std::min(sleep_ms * 100, 60 * 1000));
                }
            }
            LOG(WARNING) << "ReConn-cbk"
                << " addr=" << addr
                << " port=" << port
                << " tips=" << tips
                << " state=" << ToString(GetState())
                << " ok=" << ok;

            retryMutex.unlock();
            return ok;
        };
        return threadPool->MoveToThread(cbk);
    }

    std::future<bool> Write(Buffer buffer)
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
            return MakeFuture(false);
        }
        LOG(INFO) << __func__ 
            << " addr=" << addr 
            << " port=" << port 
            << " tips=" << tips
            << " state=" << ToString(GetState())
            << " bufferSize=" << buffer->size();

        auto cbk = [=]
        {
            auto future = client->Write(buffer);
            auto err = future.get();
            if (err->first)
            {
                // err
                LOG(ERROR) << "Write-cbk" 
                    << " addr=" << addr
                    << " port=" << port
                    << " tips=" << tips
                    << " state=" << ToString(GetState())
                    << " bufferSize=" << buffer->size()
                    << " errCode=" << err->second
                    << " errMsg" << err->second;
                SetState(ConnectionStates::NetError);
                if (retryConn)
                    ReConn();
                return false;
            }
            LOG(INFO) << "Write-cbk"  
                << " addr=" << addr
                << " port=" << port
                << " tips=" << tips
                << " state=" << ToString(GetState())
                << " bufferSize=" << buffer->size();
            return true;
        };
        return threadPool->MoveToThread(cbk);
    }

    void OnRead(Error err, Buffer buffer)
    {
        auto cbk = [=]
        {
            if (err->first)
            {
                // err
                LOG(ERROR) << "OnRead-cbk"
                    << " addr=" << addr
                    << " port=" << port 
                    << " tips=" << tips
                    << " state=" << ToString(GetState())
                    << " bufferSize=" << buffer->size()
                    << " errCode=" << err->second
                    << " errMsg" << err->second;
                SetState(ConnectionStates::NetError);
                if (retryConn)
                    ReConn();
                return false;
            }
            // success
            LOG(INFO) << "OnRead-cbk" 
                << " addr=" << addr
                << " port=" << port
                << " tips=" << tips
                << " state=" << ToString(GetState())
                << " bufferSize=" << buffer->size();
            if (handleRead)
                handleRead(buffer);
            return true;
        };
        threadPool->MoveToThread(cbk);
    }
};
