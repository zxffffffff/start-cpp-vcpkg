/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#include "SampleDataCenter.h"
#include "gtest/gtest.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable:4566)
#endif

TEST(SampleDataCenter, Test1) {
  EXPECT_EQ(SampleDataCenter::Test(123), 123);
}

TEST(SampleDataCenter, Test2) {
  ASSERT_EQ(123, 123);
}
