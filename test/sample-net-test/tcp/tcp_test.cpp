/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#include <gtest/gtest.h>
#include "tcp/tcp_server.h"
#include "tcp/tcp_client.h"
#include "tcp/impl/libuv_tcp_server_impl.h"
#include "tcp/impl/libuv_tcp_client_impl.h"
#include "thread/impl/stl_threadpool_impl.h"

using namespace std::chrono_literals;

using TestTcpServer = TcpServer<ServerImpl, ThreadPoolImpl>;
using TestTcpClient = TcpClient<ClientImpl, ThreadPoolImpl>;

struct Data
{
    std::mutex mutex; // std::lock_guard<std::mutex> guard(mutex);
    std::vector<std::string> buffer;
};

TEST(tcp, pingpong)
{
    if (!google::IsGoogleLoggingInitialized())
        google::InitGoogleLogging("test");
    FLAGS_logtostderr = false;

    // server
    auto server = std::make_shared<TestTcpServer>();
    auto server_recv = std::make_shared<Data>();
    auto serverRead = [=](ConnId connId, Buffer buffer)
    {
        // [2] -> pingpong ->
        {
            std::lock_guard<std::mutex> guard(server_recv->mutex);
            server_recv->buffer.push_back(buffer->data());
        }
        server->Write(connId, buffer);
    };
    server->SetHandleConnRead(serverRead);
    ASSERT_EQ(server->GetState(), ServerStates::Closed);
    server->ListenSync();
    ASSERT_EQ(server->GetState(), ServerStates::Listening);

    // client
    constexpr int cnt = 3;
    auto clients = std::make_shared<std::vector<TestTcpClient>>(cnt);
    auto client_recv = std::make_shared<Data>();
    for (int i = 0; i < cnt; ++i)
    {
        auto client = &(*clients)[i];
        auto clientRead = [&](Buffer buffer)
        {
            // [3] -> pong
            std::lock_guard<std::mutex> guard(client_recv->mutex);
            client_recv->buffer.push_back(buffer->data());
        };
        client->SetHandleRead(clientRead);
        ASSERT_EQ(client->GetState(), ConnectionStates::Closed);
        client->ConnectSync();
        ASSERT_EQ(client->GetState(), ConnectionStates::Connected);
    }

    for (int i = 0; i < cnt; ++i)
    {
        // [1] ping ->
        auto client = &(*clients)[i];
        std::string msg = std::to_string(1000 + i);
        client->Write(MakeBuffer(msg));
    }

    std::this_thread::sleep_for(100ms + cnt * 10ms);

    EXPECT_EQ(server_recv->buffer.size(), cnt);
    std::sort(server_recv->buffer.begin(), server_recv->buffer.end());
    std::sort(client_recv->buffer.begin(), client_recv->buffer.end());
    EXPECT_EQ(server_recv->buffer, client_recv->buffer);


    std::vector<std::future<bool>> closeFuture;
    for (int i = 0; i < cnt; ++i)
    {
        auto client = &(*clients)[i];
        EXPECT_EQ(client->GetState(), ConnectionStates::Connected);
        closeFuture.push_back(client->Close());
    }
    EXPECT_EQ(server->GetState(), ServerStates::Listening);
    server->CloseSync();
    EXPECT_EQ(server->GetState(), ServerStates::Closed);
    for (int i = 0; i < cnt; ++i)
    {
        auto client = &(*clients)[i];
        auto& future = closeFuture[i];
        future.wait();
        EXPECT_EQ(client->GetState(), ConnectionStates::Closed);
    }
}

TEST(tcp, monkeytest)
{
    if (!google::IsGoogleLoggingInitialized())
        google::InitGoogleLogging("test");
    FLAGS_logtostderr = false;

    // server
    auto server = std::make_shared<TestTcpServer>();
    auto server_recv = std::make_shared<Data>();
    auto serverRead = [=](ConnId connId, Buffer buffer)
    {
        // [2] -> pingpong ->
        {
            std::lock_guard<std::mutex> guard(server_recv->mutex);
            server_recv->buffer.push_back(buffer->data());
        }
        server->Write(connId, buffer);
    };
    server->SetHandleConnRead(serverRead);
    server->ListenSync();

    // client
    constexpr int client_cnt = 10;
    auto clients = std::make_shared<std::vector<TestTcpClient>>(client_cnt);
    auto client_recv = std::make_shared<std::vector<Data>>(client_cnt);
    for (int i = 0; i < client_cnt; ++i)
    {
        auto client = &(*clients)[i];
        auto clientRead = [&, i](Buffer buffer)
        {
            // [3] -> pong
            auto& data = (*client_recv)[i];
            std::lock_guard<std::mutex> guard(data.mutex);
            data.buffer.push_back(buffer->data());
        };
        client->SetHandleRead(clientRead);
    }
    auto ClientConnectClose = [&](int i)
    {
        auto client = &(*clients)[i];
        if (client->IsRunning())
            client->CloseSync();
        else
            client->ConnectSync();
    };
    auto ClientWrite = [&](int i)
    {
        // [1] ping ->
        auto client = &(*clients)[i];
        static std::atomic_int index;
        client->Write(MakeBuffer(std::to_string(100000 * i + index)));
        ++index;
    };

    static std::atomic_bool thread_run = true;
    auto test = [&] (int index)
    {
        std::vector<std::function<void(int)>> client_cmd;
        for (int i = 0; i < 2; ++i) client_cmd.push_back(ClientConnectClose);
        for (int i = 0; i < 8; ++i) client_cmd.push_back(ClientWrite);

        std::srand(std::time(nullptr));
        while (thread_run)
        {
            int random = std::rand() % client_cmd.size();
            auto f = client_cmd[random];
            f(index);
            std::this_thread::sleep_for(5ms);
        }
    };
    std::vector<std::thread> threads;
    for (int i = 0; i < client_cnt; ++i)
        threads.push_back(std::thread(test, i));
    std::this_thread::sleep_for(5s);
    thread_run = false;
    for (int i = 0; i < client_cnt; ++i)
        threads[i].join();

    EXPECT_TRUE(server_recv->buffer.size() > 0);

    int client_size = 0;
    for (int i = 0; i < client_cnt; ++i)
    {
        auto recv = &(*client_recv)[i];
        client_size += recv->buffer.size();
    }
    EXPECT_TRUE(client_size > 0);

    std::vector<std::future<bool>> closeFuture;
    for (int i = 0; i < client_cnt; ++i)
    {
        auto client = &(*clients)[i];
        closeFuture.push_back(client->Close());
    }
    server->CloseSync();
    EXPECT_EQ(server->GetState(), ServerStates::Closed);
    for (int i = 0; i < client_cnt; ++i)
    {
        auto client = &(*clients)[i];
        auto& future = closeFuture[i];
        future.wait();
        EXPECT_EQ(client->GetState(), ConnectionStates::Closed);
    }
}

TEST(tcp, monkeytest2)
{
    if (!google::IsGoogleLoggingInitialized())
        google::InitGoogleLogging("test");
    FLAGS_logtostderr = false;

    // server
    auto server = std::make_shared<TestTcpServer>();
    auto server_recv = std::make_shared<Data>();
    auto serverRead = [=](ConnId connId, Buffer buffer)
    {
        // [3] -> pong
        std::lock_guard<std::mutex> guard(server_recv->mutex);
        server_recv->buffer.push_back(buffer->data());
    };
    server->SetHandleConnRead(serverRead);
    server->ListenSync();
    auto ServerListenClose = [&]
    {
        if (server->IsRunning())
            server->CloseSync();
        else
            server->ListenSync();
    };
    auto ServerWrite = [&]
    {
        // [1] ping ->
        static std::atomic_int index;
        server->Write(MakeBuffer(std::to_string(index)));
        ++index;
    };

    // client
    constexpr int client_cnt = 10;
    auto clients = std::make_shared<std::vector<TestTcpClient>>(client_cnt);
    auto client_recv = std::make_shared<std::vector<Data>>(client_cnt);
    for (int i = 0; i < client_cnt; ++i)
    {
        auto client = &(*clients)[i];
        auto clientRead = [&, i](Buffer buffer)
        {
            // [2] -> pingpong ->
            auto& data = (*client_recv)[i];
            {
                std::lock_guard<std::mutex> guard(data.mutex);
                data.buffer.push_back(buffer->data());
            }
            client->Write(buffer);
        };
        client->SetHandleRead(clientRead);
        client->SetRetryConnect(true);
        client->Connect();
    }

    static std::atomic_bool thread_run = true;
    auto test = [&]
    {
        std::vector<std::function<void()>> server_cmd;
        for (int i = 0; i < 2; ++i) server_cmd.push_back(ServerListenClose);
        for (int i = 0; i < 8; ++i) server_cmd.push_back(ServerWrite);

        std::srand(std::time(nullptr));
        while (thread_run)
        {
            int random = std::rand() % server_cmd.size();
            auto f = server_cmd[random];
            f();
            std::this_thread::sleep_for(5ms);
        }
    };
    std::thread thread(test);
    std::this_thread::sleep_for(5s);
    thread_run = false;
    thread.join();

    EXPECT_TRUE(server_recv->buffer.size() > 0);

    int client_size = 0;
    for (int i = 0; i < client_cnt; ++i)
    {
        auto recv = &(*client_recv)[i];
        client_size += recv->buffer.size();
    }
    EXPECT_TRUE(client_size > 0);

    std::vector<std::future<bool>> closeFuture;
    for (int i = 0; i < client_cnt; ++i)
    {
        auto client = &(*clients)[i];
        client->SetRetryConnect(false);
        closeFuture.push_back(client->Close());
    }
    server->CloseSync();
    EXPECT_EQ(server->GetState(), ServerStates::Closed);
    for (int i = 0; i < client_cnt; ++i)
    {
        auto client = &(*clients)[i];
        auto& future = closeFuture[i];
        future.wait();
        EXPECT_EQ(client->GetState(), ConnectionStates::Closed);
    }
}
