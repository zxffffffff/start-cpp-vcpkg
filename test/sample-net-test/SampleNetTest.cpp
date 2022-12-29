#include "SampleNet.h"
#include "gtest/gtest.h"

TEST(SampleNet, Test1) {
  EXPECT_EQ(SampleNet::Test(123), 123);
}

TEST(SampleNet, Test2) {
  ASSERT_EQ(123, 123);
}
