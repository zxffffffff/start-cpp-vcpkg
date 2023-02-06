#include "tcp/TcpServer.h"
#include "tcp/TcpClient.h"
#include "gtest/gtest.h"
#include <thread>

TEST(TcpConnTest, WriteAndRead) {
    using namespace std::chrono_literals;

    TcpServer server("0.0.0.0", 9123);
    server.Run();
    std::this_thread::sleep_for(100ms);
    ASSERT_EQ(server.IsRunning(), true);

    TcpClient client("0.0.0.0", 9123);
    client.Run();
    std::this_thread::sleep_for(100ms);
    ASSERT_EQ(client.IsRunning(), true);

    // write & read
    std::string buff = "abc123!@#$%^&*()_-+=*/\\'\"?你好";
    static std::string buff2;
    server.SetHandleConnOnRead([](SocketPtr id, const char* buf, size_t len) {
        buff2 = std::string(buf, len);
    });
    client.Write(buff.c_str(), buff.length());
    std::this_thread::sleep_for(100ms);
    EXPECT_EQ(buff, buff2);

    client.Close();
    std::this_thread::sleep_for(500ms);
    ASSERT_EQ(client.IsRunning(), false);

    server.Close();
    std::this_thread::sleep_for(500ms);
    ASSERT_EQ(server.IsRunning(), false);
}

TEST(TcpConnTest, Test2) {
    EXPECT_EQ(123, 123);
    ASSERT_EQ(123, 123);
}
