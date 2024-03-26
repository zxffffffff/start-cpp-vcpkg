/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#include <gtest/gtest.h>

#include "common.h"
#include "tcp_server.h"
#include "tcp_client.h"
#include "impl/libuv_tcp_server_impl.h"
#include "impl/libuv_tcp_client_impl.h"
#include "impl/stl_threadpool_impl.h"
using TestTcpServer = TcpServer<ServerImpl>;
using TestTcpClient = TcpClient<ClientImpl>;

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable : 4566)
#endif

using namespace std::chrono_literals;

struct Data
{
    std::mutex mutex; // std::lock_guard<std::mutex> guard(mutex);
    std::vector<std::string> strs;
};

TEST(tcp, pingpong)
{
    // server
    auto server = std::make_shared<TestTcpServer>();
    auto server_recv = std::make_shared<Data>();
    auto serverRead = [=](ConnId connId, Buffer buffer)
    {
        // [2] -> pingpong ->
        {
            std::lock_guard<std::mutex> guard(server_recv->mutex);
            server_recv->strs.push_back(std::string(buffer->data(), buffer->size()));
        }
        server->Write(connId, buffer);
    };
    server->SetHandleConnRead(serverRead);
    ASSERT_EQ(server->GetState(), ServerState::Closed);
    server->ListenSync();
    ASSERT_EQ(server->GetState(), ServerState::Listening);

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
            client_recv->strs.push_back(std::string(buffer->data(), buffer->size()));
        };
        client->SetHandleRead(clientRead);
        ASSERT_EQ(client->GetState(), ConnectionState::Closed);
        client->ConnectSync();
        ASSERT_EQ(client->GetState(), ConnectionState::Connected);
    }

    for (int i = 0; i < cnt; ++i)
    {
        // [1] ping ->
        auto client = &(*clients)[i];
        std::string msg = std::to_string(1000 + i);
        client->Write(MakeBuffer(msg));
    }

    std::this_thread::sleep_for(500ms + cnt * 100ms);

    EXPECT_EQ(server_recv->strs.size(), cnt);
    std::sort(server_recv->strs.begin(), server_recv->strs.end());
    std::sort(client_recv->strs.begin(), client_recv->strs.end());
    EXPECT_EQ(server_recv->strs, client_recv->strs);

    for (int i = 0; i < cnt; ++i)
    {
        auto client = &(*clients)[i];
        EXPECT_EQ(client->GetState(), ConnectionState::Connected);
        client->Close();
    }
    EXPECT_EQ(server->GetState(), ServerState::Listening);
    server->CloseSync();
    EXPECT_EQ(server->GetState(), ServerState::Closed);
    for (int i = 0; i < cnt; ++i)
    {
        auto client = &(*clients)[i];
        client->WaitForState(ConnectionState::Closed, 1);
        EXPECT_EQ(client->GetState(), ConnectionState::Closed);
    }
}

TEST(tcp, monkeytest)
{
    // server
    auto server = std::make_shared<TestTcpServer>();
    auto server_recv = std::make_shared<Data>();
    auto serverRead = [=](ConnId connId, Buffer buffer)
    {
        // [2] -> pingpong ->
        {
            std::lock_guard<std::mutex> guard(server_recv->mutex);
            server_recv->strs.push_back(std::string(buffer->data(), buffer->size()));
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
            auto &data = (*client_recv)[i];
            std::lock_guard<std::mutex> guard(data.mutex);
            data.strs.push_back(std::string(buffer->data(), buffer->size()));
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

    static std::atomic<bool> thread_run{true};
    auto test = [&](int index)
    {
        std::vector<std::function<void(int)>> client_cmd;
        for (int i = 0; i < 20; ++i)
            client_cmd.push_back(ClientConnectClose); // 20%
        for (int i = 0; i < 80; ++i)
            client_cmd.push_back(ClientWrite); // 80%

        while (thread_run)
        {
            int random = Common::Random() % client_cmd.size();
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

    EXPECT_TRUE(server_recv->strs.size() > 0);

    int client_size = 0;
    for (int i = 0; i < client_cnt; ++i)
    {
        auto recv = &(*client_recv)[i];
        client_size += recv->strs.size();
    }
    EXPECT_TRUE(client_size > 0);

    for (int i = 0; i < client_cnt; ++i)
    {
        auto client = &(*clients)[i];
        client->Close();
    }
    server->CloseSync();
    EXPECT_EQ(server->GetState(), ServerState::Closed);
    for (int i = 0; i < client_cnt; ++i)
    {
        auto client = &(*clients)[i];
        client->WaitForState(ConnectionState::Closed, 1);
        EXPECT_EQ(client->GetState(), ConnectionState::Closed);
    }
}

TEST(tcp, monkeytest2)
{
    // server
    auto server = std::make_shared<TestTcpServer>();
    auto server_recv = std::make_shared<Data>();
    auto serverRead = [=](ConnId connId, Buffer buffer)
    {
        // [3] -> pong
        std::lock_guard<std::mutex> guard(server_recv->mutex);
        server_recv->strs.push_back(std::string(buffer->data(), buffer->size()));
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
            // [2] -> pingpong ->
            auto &data = (*client_recv)[i];
            {
                std::lock_guard<std::mutex> guard(data.mutex);
                data.strs.push_back(std::string(buffer->data(), buffer->size()));
            }
            client->Write(buffer);
        };
        client->SetHandleRead(clientRead);
        client->Connect();
    }

    auto ServerListenSwitch = [&]
    {
        if (server->IsRunning())
            server->CloseSync();
        else
            server->ListenSync();
        for (int i = 0; i < client_cnt; ++i)
        {
            auto client = &(*clients)[i];
            client->Connect();
        }
    };
    auto ServerWrite = [&]
    {
        // [1] ping ->
        static std::atomic_int index;
        server->Write(MakeBuffer(std::to_string(index)));
        ++index;
    };

    static std::atomic<bool> thread_run{true};
    auto test = [&]
    {
        std::vector<std::function<void()>> server_cmd;
        for (int i = 0; i < 50; ++i)
            server_cmd.push_back(ServerWrite); // 100% -> 50%

        while (thread_run)
        {
            int random = Common::Random() % server_cmd.size();
            auto f = server_cmd[random];
            f();
            std::this_thread::sleep_for(5ms);

            if (server_cmd.size() < 100)
                server_cmd.push_back(ServerListenSwitch); // 0% -> 50%
        }
    };
    std::thread thread(test);
    std::this_thread::sleep_for(5s);
    thread_run = false;
    thread.join();

    EXPECT_TRUE(server_recv->strs.size() > 0);

    int client_size = 0;
    for (int i = 0; i < client_cnt; ++i)
    {
        auto recv = &(*client_recv)[i];
        client_size += recv->strs.size();
    }
    EXPECT_TRUE(client_size > 0);

    for (int i = 0; i < client_cnt; ++i)
    {
        auto client = &(*clients)[i];
        client->Close();
    }
    server->CloseSync();
    EXPECT_EQ(server->GetState(), ServerState::Closed);
    for (int i = 0; i < client_cnt; ++i)
    {
        auto client = &(*clients)[i];
        client->WaitForState(ConnectionState::Closed, 1);
        EXPECT_EQ(client->GetState(), ConnectionState::Closed);
    }
}
