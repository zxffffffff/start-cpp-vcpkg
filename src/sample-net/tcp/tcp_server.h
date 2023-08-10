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
            handleStates(new_state);
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
            handleConnStates(connId, new_state);
    }

    /* 线程异步回调，注意线程安全 */
    void SetHandleStates(HandleServerStates f) { handleStates = f; }

    /* 线程异步回调，注意线程安全 */
    void SetHandleConnStates(HandleServerConnStates f) { handleConnStates = f; }

    /* 线程异步回调，注意线程安全 */
    void SetHandleConnRead(HandleServerConnRead f) { handleConnRead = f; }

public:
    std::future<bool> Listen()
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

        SetState(ServerStates::Listen);
        auto cbk = [this]
        {
            auto err = server->Listen(GetAddr(), GetPort()).get();
            if (err->first)
            {
                // err
                LOG(ERROR) << "Listen-cbk"
                    << " addr=" << addr
                    << " port=" << port
                    << " tips=" << tips
                    << " state=" << ToString(GetState())
                    << " Listen Failed !";
                SetState(ServerStates::ListenFailed);
                return false;
            }
            // success
            LOG(INFO) << "Listen-cbk"
                << " addr=" << addr
                << " port=" << port 
                << " tips=" << tips
                << " state=" << ToString(GetState());
            SetState(ServerStates::Listening);
            return true;
        };
        return threadPool->MoveToThread(cbk);
    }

    std::future<bool> Close()
    {
        if (GetState() <= ServerStates::Closing)
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

        SetState(ServerStates::Closing);
        auto cbk = [this]
        {
            auto err = server->Close().get();
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
            SetState(ServerStates::Closed);
            return true;
        };
        return threadPool->MoveToThread(cbk);
    }

    std::future<bool> Write(ConnId connId, Buffer buffer)
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
            return MakeFuture(false);
        }
        LOG(INFO) << __func__
            << " addr=" << addr
            << " port=" << port
            << " tips=" << tips
            << " state=" << ToString(GetState())
            << " connId=" << connId
            << " bufferSize=" << buffer->size();

        auto cbk = [=]
        {
            auto& connection = connections[connId];
            auto err = server->Write(connId, buffer).get();
            if (err->first)
            {
                // err
                LOG(ERROR) << "Write-cbk" 
                    << " addr=" << addr
                    << " port=" << port
                    << " tips=" << tips
                    << " state=" << ToString(GetState())
                    << " connId=" << connId
                    << " bufferSize=" << buffer->size()
                    << " errCode=" << err->second
                    << " errMsg" << err->second;
                SetConnState(connId, ConnectionStates::NetError);
                return false;
            }
            LOG(INFO) << "Write-cbk"
                << " addr=" << addr
                << " port=" << port
                << " tips=" << tips
                << " state=" << ToString(GetState())
                << " connId=" << connId
                << " bufferSize=" << buffer->size();
            return true;
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
            std::vector<std::future<bool>> futures;
            for (auto ite = connections.begin(); ite != connections.end(); ++ite)
            {
                ConnId connId = ite->first;
                futures.push_back(Write(connId, buffer));
            }

            bool ok = false;
            for (auto& future : futures)
            {
                if (future.get())
                    ok = true;
            }
            LOG(INFO) << "Write-cbk"
                << " addr=" << addr
                << " port=" << port
                << " tips=" << tips
                << " state=" << ToString(GetState())
                << " bufferSize=" << buffer->size()
                << " ok=" << ok;
            return ok;
        };
        return threadPool->MoveToThread(cbk);
    }

    void OnNewConn(ConnId connId)
    {
        auto cbk = [=]
        {
            LOG(INFO) << "OnNewConn-cbk"
                << " addr=" << addr
                << " port=" << port
                << " tips=" << tips
                << " state=" << ToString(GetState())
                << " connId=" << connId;
            SetConnState(connId, ConnectionStates::Connected);
            return true;
        };
        threadPool->MoveToThread(cbk);
    }

    void OnCloseConn(ConnId connId)
    {
        auto cbk = [=]
        {
            LOG(INFO) << "OnCloseConn-cbk"
                << " addr=" << addr
                << " port=" << port
                << " tips=" << tips
                << " state=" << ToString(GetState())
                << " connState=" << ToString(GetConnState(connId))
                << " connId=" << connId;
            SetConnState(connId, ConnectionStates::Closed);
            return true;
        };
        threadPool->MoveToThread(cbk);
    }

    void OnConnRead(ConnId connId, Error err, Buffer buffer)
    {
        auto cbk = [=]
        {
            if (err->first)
            {
                // err
                LOG(ERROR) << "OnConnRead-cbk"
                    << " addr=" << addr
                    << " port=" << port
                    << " tips=" << tips
                    << " state=" << ToString(GetState())
                    << " connState=" << ToString(GetConnState(connId))
                    << " connId=" << connId
                    << " bufferSize=" << buffer->size()
                    << " errCode=" << err->second
                    << " errMsg" << err->second;
                SetConnState(connId, ConnectionStates::NetError);
                return false;
            }
            // success
            LOG(INFO) << "OnConnRead-cbk"
                << " addr=" << addr
                << " port=" << port
                << " tips=" << tips
                << " state=" << ToString(GetState())
                << " connState=" << ToString(GetConnState(connId))
                << " connId=" << connId
                << " bufferSize=" << buffer->size();
            if (handleConnRead)
                handleConnRead(connId, buffer);
            return true;
        };
        threadPool->MoveToThread(cbk);
    }
};
