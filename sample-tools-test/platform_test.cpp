/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#include <gtest/gtest.h>
#include "platform.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable : 4566)
#endif

TEST(utf8, test)
{
    std::string utf8 = u8"abc123!@#$%^&*()_-+=*/\\'‘\"“?？:：,，.。你好～";
    std::string local = Platform::utf8_to_local(utf8);
    std::string utf8_2 = Platform::local_to_utf8(local);
    std::string local_2 = Platform::utf8_to_local(utf8_2);

    EXPECT_EQ(utf8, utf8_2);
    EXPECT_EQ(local, local_2);
}

TEST(os, test)
{
    std::string type = Platform::GetOSType();
    std::string ver = Platform::GetOSVersion();

    EXPECT_TRUE(type.size());
    EXPECT_TRUE(ver.size());
}
