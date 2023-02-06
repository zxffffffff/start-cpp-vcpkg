#include "tcp/TcpServer.h"
#include "tcp/TcpClient.h"
#include "gtest/gtest.h"
#include <thread>

using namespace std::chrono_literals;

TEST(TcpConnTest, ClientWrite) {
    static std::string buff;

    // run
    TcpServer server("0.0.0.0", 9123);
    server.SetHandleConnOnRead([](SocketPtr id, const char* buf, size_t len) {
        buff = std::string(buf, len);
    });
    server.Run();
    std::this_thread::sleep_for(100ms);
    ASSERT_EQ(server.IsRunning(), true);

    TcpClient client("0.0.0.0", 9123);
    client.Run();
    std::this_thread::sleep_for(100ms);
    ASSERT_EQ(client.IsRunning(), true);

    // write & read
    std::string buff_c = "abc123!@#$%^&*()_-+=*/\\'\"?你好";
    client.Write(buff_c.c_str(), buff_c.length());
    std::this_thread::sleep_for(100ms);
    EXPECT_EQ(buff, buff_c);

    // close
    client.Close();
    std::this_thread::sleep_for(500ms);
    ASSERT_EQ(client.IsRunning(), false);

    server.Close();
    std::this_thread::sleep_for(500ms);
    ASSERT_EQ(server.IsRunning(), false);
}

TEST(TcpConnTest, MultiClientWrite) {
    static std::set<SocketPtr> conn;
    static SocketPtr id_c1 = 0, id_c2 = 0, id_c3 = 0;

    // run
    TcpServer server("0.0.0.0", 9124);
    server.SetHandleNewConn([](SocketPtr id) {
        conn.insert(id);
    });
    server.SetHandleConnOnRead([](SocketPtr id, const char* buf, size_t len) {
        conn.erase(id);
        std::string buff(buf, len);
        if (buff.starts_with("1_"))
            EXPECT_TRUE(buff.ends_with(std::to_string((size_t)id_c1)));
        else if (buff.starts_with("2_"))
            EXPECT_TRUE(buff.ends_with(std::to_string((size_t)id_c2)));
        else if (buff.starts_with("3_"))
            EXPECT_TRUE(buff.ends_with(std::to_string((size_t)id_c3)));
    });
    server.Run();
    std::this_thread::sleep_for(100ms);
    ASSERT_EQ(server.IsRunning(), true);

    TcpClient client1("0.0.0.0", 9124);
    client1.SetHandleNewConn([](SocketPtr id) {
        id_c1 = id;
    });
    client1.Run();

    TcpClient client2("0.0.0.0", 9124);
    client2.SetHandleNewConn([](SocketPtr id) {
        id_c2 = id;
    });
    client2.Run();

    TcpClient client3("0.0.0.0", 9124);
    client3.SetHandleNewConn([](SocketPtr id) {
        id_c3 = id;
    });
    client3.Run();

    std::this_thread::sleep_for(100ms);
    ASSERT_EQ(client1.IsRunning(), true);
    ASSERT_EQ(client2.IsRunning(), true);
    ASSERT_EQ(client3.IsRunning(), true);
    ASSERT_EQ(conn.size(), 3);

    // write & read
    std::string buff_c1 = "1_" + std::to_string((size_t)id_c1);
    std::string buff_c2 = "2_" + std::to_string((size_t)id_c2);
    std::string buff_c3 = "3_" + std::to_string((size_t)id_c3);
    client1.Write(buff_c1.c_str(), buff_c1.length());
    client2.Write(buff_c2.c_str(), buff_c2.length());
    client3.Write(buff_c3.c_str(), buff_c3.length());
    std::this_thread::sleep_for(100ms);
    ASSERT_EQ(conn.size(), 0);

    // close
    client1.Close();
    client2.Close();
    client3.Close();
    std::this_thread::sleep_for(500ms);
    ASSERT_EQ(client1.IsRunning(), false);
    ASSERT_EQ(client2.IsRunning(), false);
    ASSERT_EQ(client3.IsRunning(), false);

    server.Close();
    std::this_thread::sleep_for(500ms);
    ASSERT_EQ(server.IsRunning(), false);
}
