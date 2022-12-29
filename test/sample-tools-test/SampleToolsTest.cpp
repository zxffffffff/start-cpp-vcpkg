#include "SampleTools.h"
#include "gtest/gtest.h"

TEST(SampleTools, Test1) {
  EXPECT_EQ(SampleTools::Test(123), 123);
}

TEST(SampleTools, Test2) {
  ASSERT_EQ(123, 123);
}
