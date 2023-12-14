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
    std::string msg = "abc123!@#$%^&*()_-+=*/\\'\"?你好";
    std::string local = Platform::utf8_to_local(msg);
    std::string utf8 = Platform::local_to_utf8(local);

    EXPECT_EQ(msg, utf8);
}

TEST(macAddr, test)
{
    std::string mac = Platform::GetMacAddr();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::string mac2 = Platform::GetMacAddr();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::string mac3 = Platform::GetMacAddr();

    EXPECT_TRUE(mac.size());
    EXPECT_EQ(mac, mac2);
    EXPECT_EQ(mac, mac3);
}

TEST(os, test)
{
    std::string type = Platform::GetOSType();
    std::string ver = Platform::GetOSVersion();

    EXPECT_TRUE(type.size());
    EXPECT_TRUE(ver.size());
}

TEST(guid, test)
{
    std::set<std::string> guids;
    const int cnt = 1000;
    for (int i = 0; i < cnt; ++i)
    {
        guids.insert(Platform::GenGuid());
    }
    ASSERT_EQ(guids.size(), cnt);
}
