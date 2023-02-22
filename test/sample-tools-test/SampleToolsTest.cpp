/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/

#include "SampleTools.h"
#include "gtest/gtest.h"

TEST(SampleTools, Test1) {
  EXPECT_EQ(SampleTools::to_lower("ABC_123_abc"), "abc_123_abc");
}

TEST(SampleTools, Test2) {
  ASSERT_EQ(123, 123);
}
