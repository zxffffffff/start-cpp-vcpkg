/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/

#include "SampleNet.h"
#include "gtest/gtest.h"

TEST(SampleNet, Test1) {
  EXPECT_EQ(SampleNet::Test(123), 123);
}

TEST(SampleNet, Test2) {
  ASSERT_EQ(123, 123);
}
