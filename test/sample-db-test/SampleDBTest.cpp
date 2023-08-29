/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#include "SampleDB.h"
#include "gtest/gtest.h"

TEST(SampleDB, Test1) {
  EXPECT_EQ(SampleDB::Test(123), 123);
}

TEST(SampleDB, Test2) {
  ASSERT_EQ(123, 123);
}
