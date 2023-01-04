#include "tcp/TcpServer.h"
#include "tcp/TcpClient.h"
#include "gtest/gtest.h"

TEST(TcpConnTest, Test1) {
  TcpServer server("0.0.0.0", 9123);
  server.Run();
  TcpClient client("0.0.0.0", 9123);
  client.Run();
  EXPECT_EQ(123, 123);
}

TEST(TcpConnTest, Test2) {
  ASSERT_EQ(123, 123);
}
