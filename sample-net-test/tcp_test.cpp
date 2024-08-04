/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#include <gtest/gtest.h>

#include "common.h"
#include "hardware.h"
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

/* 警告：Google Test 仅在 *nix 上线程安全，Windows 或其他平台不支持多线程断言 */
TEST(TcpCS, pingpong)
{
    int sleep_ms = 1000 / Hardware::GetCPUs();
    sleep_ms = std::min(std::max(100, sleep_ms), 1000);

    /* 随机端口，减少端口被占用概率 */
    std::string str_ip = "127.0.0.1";
    int n_port = Common::Random(10000, 49151);

    // server
    auto server = std::make_shared<TestTcpServer>(str_ip, n_port);
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
    std::vector<std::shared_ptr<TestTcpClient>> clients(cnt);
    for (int i = 0; i < cnt; ++i)
        clients[i] = std::make_shared<TestTcpClient>(str_ip, n_port);
    auto client_recv = std::make_shared<Data>();
    for (int i = 0; i < cnt; ++i)
    {
        auto client = clients[i];
        auto clientRead = [=](Buffer buffer)
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
        auto client = clients[i];
        std::string msg = std::to_string(1000 + i);
        client->Write(MakeBuffer(msg));
    }

    Common::Sleep(cnt * sleep_ms * 1.5);

    EXPECT_EQ(server_recv->strs.size(), cnt);
    std::sort(server_recv->strs.begin(), server_recv->strs.end());
    std::sort(client_recv->strs.begin(), client_recv->strs.end());
    EXPECT_EQ(server_recv->strs, client_recv->strs);

    for (int i = 0; i < cnt; ++i)
    {
        auto client = clients[i];
        EXPECT_EQ(client->GetState(), ConnectionState::Connected);
        client->Close();
    }
    EXPECT_EQ(server->GetState(), ServerState::Listening);
    server->CloseSync();
    EXPECT_EQ(server->GetState(), ServerState::Closed);
    for (int i = 0; i < cnt; ++i)
    {
        auto client = clients[i];
        client->WaitForState(ConnectionState::Closed, 1);
        EXPECT_EQ(client->GetState(), ConnectionState::Closed);
    }
}

/* 警告：Google Test 仅在 *nix 上线程安全，Windows 或其他平台不支持多线程断言 */
TEST(TcpCS, monkeytest)
{
    int sleep_ms = 1000 / Hardware::GetCPUs();
    sleep_ms = std::min(std::max(100, sleep_ms), 1000);

    /* 随机端口，减少端口被占用概率 */
    std::string str_ip = "127.0.0.1";
    int n_port = Common::Random(10000, 49151);

    // server
    auto server = std::make_shared<TestTcpServer>(str_ip, n_port);
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
    std::vector<std::shared_ptr<TestTcpClient>> clients(client_cnt);
    std::vector<std::shared_ptr<Data>> client_recv(client_cnt);
    for (int i = 0; i < client_cnt; ++i)
    {
        clients[i] = std::make_shared<TestTcpClient>(str_ip, n_port);
        client_recv[i] = std::make_shared<Data>();
    }
    for (int i = 0; i < client_cnt; ++i)
    {
        auto client = clients[i];
        auto clientRead = [=](Buffer buffer)
        {
            // [3] -> pong
            auto &data = client_recv[i];
            std::lock_guard<std::mutex> guard(data->mutex);
            data->strs.push_back(std::string(buffer->data(), buffer->size()));
        };
        client->SetHandleRead(clientRead);
    }
    auto ClientConnectClose = [=](int i)
    {
        auto client = clients[i];
        if (client->IsRunning())
            client->CloseSync();
        else
            client->ConnectSync();
    };
    auto ClientWrite = [=](int i)
    {
        // [1] ping ->
        auto client = clients[i];
        static std::atomic_int index;
        client->Write(MakeBuffer(std::to_string(100000 * i + index)));
        ++index;
    };

    static std::atomic<bool> thread_run{true};
    auto test = [=](int index)
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
            Common::Sleep(sleep_ms / 10);
        }
    };
    std::vector<std::thread> threads;
    for (int i = 0; i < client_cnt; ++i)
        threads.push_back(std::thread(test, i));
    std::this_thread::sleep_for(10s);
    thread_run = false;
    for (int i = 0; i < client_cnt; ++i)
        threads[i].join();

    EXPECT_TRUE(server_recv->strs.size() > 0);

    int client_size = 0;
    for (int i = 0; i < client_cnt; ++i)
    {
        auto recv = client_recv[i];
        client_size += recv->strs.size();
    }
    EXPECT_TRUE(client_size > 0);

    for (int i = 0; i < client_cnt; ++i)
    {
        auto client = clients[i];
        client->Close();
    }
    server->CloseSync();
    EXPECT_EQ(server->GetState(), ServerState::Closed);
    for (int i = 0; i < client_cnt; ++i)
    {
        auto client = clients[i];
        client->WaitForState(ConnectionState::Closed, 1);
        EXPECT_EQ(client->GetState(), ConnectionState::Closed);
    }
}

/* 警告：Google Test 仅在 *nix 上线程安全，Windows 或其他平台不支持多线程断言 */
TEST(TcpCS, monkeytest2)
{
    int sleep_ms = 1000 / Hardware::GetCPUs();
    sleep_ms = std::min(std::max(100, sleep_ms), 1000);

    /* 随机端口，减少端口被占用概率 */
    std::string str_ip = "127.0.0.1";
    int n_port = Common::Random(10000, 49151);

    // server
    auto server = std::make_shared<TestTcpServer>(str_ip, n_port);
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
    std::vector<std::shared_ptr<TestTcpClient>> clients(client_cnt);
    std::vector<std::shared_ptr<Data>> client_recv(client_cnt);
    for (int i = 0; i < client_cnt; ++i)
    {
        clients[i] = std::make_shared<TestTcpClient>(str_ip, n_port);
        client_recv[i] = std::make_shared<Data>();
    }
    for (int i = 0; i < client_cnt; ++i)
    {
        auto client = clients[i];
        auto clientRead = [=](Buffer buffer)
        {
            // [2] -> pingpong ->
            auto data = client_recv[i];
            {
                std::lock_guard<std::mutex> guard(data->mutex);
                data->strs.push_back(std::string(buffer->data(), buffer->size()));
            }
            client->Write(buffer);
        };
        client->SetHandleRead(clientRead);
        client->Connect();
    }

    auto ServerListenSwitch = [=]
    {
        if (server->IsRunning())
            server->CloseSync();
        else
            server->ListenSync();
        for (int i = 0; i < client_cnt; ++i)
        {
            auto client = clients[i];
            client->Connect();
        }
    };
    auto ServerWrite = [=]
    {
        // [1] ping ->
        static std::atomic_int index;
        server->Write(MakeBuffer(std::to_string(index)));
        ++index;
    };

    static std::atomic<bool> thread_run{true};
    auto test = [=]
    {
        std::vector<std::function<void()>> server_cmd;
        for (int i = 0; i < 50; ++i)
            server_cmd.push_back(ServerWrite); // 100% -> 50%

        while (thread_run)
        {
            int random = Common::Random() % server_cmd.size();
            auto f = server_cmd[random];
            f();
            Common::Sleep(sleep_ms / 10);

            if (server_cmd.size() < 100)
                server_cmd.push_back(ServerListenSwitch); // 0% -> 50%
        }
    };
    std::thread thread(test);
    std::this_thread::sleep_for(10s);
    thread_run = false;
    thread.join();

    EXPECT_TRUE(server_recv->strs.size() > 0);

    int client_size = 0;
    for (int i = 0; i < client_cnt; ++i)
    {
        auto recv = client_recv[i];
        client_size += recv->strs.size();
    }
    EXPECT_TRUE(client_size > 0);
    for (int i = 0; i < client_cnt; ++i)
    {
        auto client = clients[i];
        client->Close();
    }
    server->CloseSync();
    EXPECT_EQ(server->GetState(), ServerState::Closed);
    for (int i = 0; i < client_cnt; ++i)
    {
        auto client = clients[i];
        client->WaitForState(ConnectionState::Closed, 1);
        EXPECT_EQ(client->GetState(), ConnectionState::Closed);
    }
}
