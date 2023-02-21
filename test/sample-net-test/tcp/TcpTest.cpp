/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/

#include "tcp/TcpServer.h"
#include "tcp/TcpClient.h"
#include "gtest/gtest.h"

using namespace std::chrono_literals;

TEST(TcpConnTest, ClientWrite)
{
    if (!google::IsGoogleLoggingInitialized())
        google::InitGoogleLogging("test");

    static std::string buff;

    // run
    TcpServer server("0.0.0.0", 9123);
    server.SetHandleConnOnRead([](SocketPtr id, const char *buf, size_t len)
                               { buff = std::string(buf, len); });
    server.Run();
    while (!server.IsRunning())
    {
        std::this_thread::sleep_for(50ms);
    }

    TcpClient client("0.0.0.0", 9123);
    client.Run();
    while (!client.IsRunning())
    {
        std::this_thread::sleep_for(50ms);
    }

    // write & read
    std::string buff_c = "abc123!@#$%^&*()_-+=*/\\'\"?你好";
    client.Write(buff_c.c_str(), buff_c.length());
    std::this_thread::sleep_for(100ms);
    EXPECT_EQ(buff, buff_c);

    // close
    client.Close();
    server.Close();
    while (client.IsRunning() || server.IsRunning())
    {
        std::this_thread::sleep_for(50ms);
    }
}

TEST(TcpConnTest, ServerWrite)
{
    if (!google::IsGoogleLoggingInitialized())
        google::InitGoogleLogging("test");

    static std::string buff;
    static SocketPtr conn_id = 0;

    // run
    TcpServer server("0.0.0.0", 9123);
    server.SetHandleNewConn([](SocketPtr id)
                            { conn_id = id; });
    server.Run();
    while (!server.IsRunning())
    {
        std::this_thread::sleep_for(50ms);
    }

    TcpClient client("0.0.0.0", 9123);
    client.SetHandleConnOnRead([](SocketPtr id, const char *buf, size_t len)
                               { buff = std::string(buf, len); });
    client.Run();
    while (!client.IsRunning())
    {
        std::this_thread::sleep_for(50ms);
    }

    // write & read
    std::string buff_c = "abc123!@#$%^&*()_-+=*/\\'\"?你好";
    server.Write(conn_id, buff_c.c_str(), buff_c.length());
    std::this_thread::sleep_for(100ms);
    EXPECT_EQ(buff, buff_c);

    // close
    client.Close();
    server.Close();
    while (client.IsRunning() || server.IsRunning())
    {
        std::this_thread::sleep_for(50ms);
    }
}

TEST(TcpConnTest, MultiPingPong)
{
    if (!google::IsGoogleLoggingInitialized())
        google::InitGoogleLogging("test");

    // run
    static TcpServer server("0.0.0.0", 9123);
    server.SetHandleConnOnRead([](SocketPtr id, const char *buf, size_t len)
                               {
        std::string buff(buf, len);
        int i = std::stoi(buff);
        buff = std::to_string(i + 1);
        server.Write(id, buff.data(), buff.length()); });
    server.Run();
    while (!server.IsRunning())
    {
        std::this_thread::sleep_for(50ms);
    }

    static TcpClient client1("0.0.0.0", 9123);
    client1.SetHandleConnOnRead([](SocketPtr id, const char *buf, size_t len)
                                {
        std::string buff(buf, len);
        int i = std::stoi(buff);
        buff = std::to_string(i + 1);
        client1.Write(buff.data(), buff.length()); });

    static TcpClient client2("0.0.0.0", 9123);
    client2.SetHandleConnOnRead([](SocketPtr id, const char *buf, size_t len)
                                {
        std::string buff(buf, len);
        int i = std::stoi(buff);
        buff = std::to_string(i + 1);
        client2.Write(buff.data(), buff.length()); });

    static TcpClient client3("0.0.0.0", 9123);
    client3.SetHandleConnOnRead([](SocketPtr id, const char *buf, size_t len)
                                {
        std::string buff(buf, len);
        int i = std::stoi(buff);
        buff = std::to_string(i + 1);
        client3.Write(buff.data(), buff.length()); });

    client1.Run();
    client2.Run();
    client3.Run();
    while (!client1.IsRunning() || !client2.IsRunning() || !client3.IsRunning())
    {
        std::this_thread::sleep_for(50ms);
    }

    // write & read
    std::string buff_c1 = "100000";
    std::string buff_c2 = "200000";
    std::string buff_c3 = "300000";
    client1.Write(buff_c1.c_str(), buff_c1.length());
    client2.Write(buff_c2.c_str(), buff_c2.length());
    client3.Write(buff_c3.c_str(), buff_c3.length());
    std::this_thread::sleep_for(333ms);

    // close
    client1.Close();
    client2.Close();
    client3.Close();
    server.Close();
    while (client1.IsRunning() || client2.IsRunning() || client3.IsRunning() || server.IsRunning())
    {
        std::this_thread::sleep_for(50ms);
    }
}
