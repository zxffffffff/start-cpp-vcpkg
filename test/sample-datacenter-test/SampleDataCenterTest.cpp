#include "SampleDataCenter.h"
#include "gtest/gtest.h"

TEST(SampleDataCenter, Test1) {
  EXPECT_EQ(SampleDataCenter::Test(123), 123);
}

TEST(SampleDataCenter, Test2) {
  ASSERT_EQ(123, 123);
}
