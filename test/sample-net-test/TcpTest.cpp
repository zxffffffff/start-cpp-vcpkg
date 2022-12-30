#include "TcpServer.h"
#include "TcpClient.h"
#include "gtest/gtest.h"

TEST(TcpConnTest, Test1) {
  TcpServer server;
  TcpClient client;
  EXPECT_EQ(123, 123);
}

TEST(TcpConnTest, Test2) {
  ASSERT_EQ(123, 123);
}
