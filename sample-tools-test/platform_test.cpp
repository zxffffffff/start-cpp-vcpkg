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

TEST(utf8, en)
{
    std::string utf8 = u8"abcABC123~!@#$%^&*()_+-={}[]|\\:;\"\'<>,.?/";
    std::string local = Platform::utf8_to_local(utf8);
    std::string utf8_2 = Platform::local_to_utf8(local);
    std::string local_2 = Platform::utf8_to_local(utf8_2);

    EXPECT_EQ(utf8, utf8_2);
    EXPECT_EQ(local, local_2);
}

TEST(utf8, cn)
{
#ifdef _WIN32
    /* 获取系统的 ANSI 代码页 */
    UINT codePage = GetACP();
    /* 检查是否是 GB2312 (CP936) 中文操作系统，可能是 Windows-1252 (CP1252) */
    if (codePage != 936)
        return;
#endif

    std::string utf8 = u8"你好，世界！";
    std::string local = Platform::utf8_to_local(utf8);
    std::string utf8_2 = Platform::local_to_utf8(local);
    std::string local_2 = Platform::utf8_to_local(utf8_2);

    EXPECT_EQ(utf8, utf8_2);
    EXPECT_EQ(local, local_2);
}

TEST(os, type)
{
    std::string type = Platform::GetOSType();
    std::cout << "os-type: " << type << std::endl;

#ifdef _WIN32
    EXPECT_EQ(type, "Windows");
#elif __APPLE__
    EXPECT_EQ(type, "macOS");
#elif __linux__
    EXPECT_EQ(type, "Linux");
#endif
}

TEST(os, version)
{
    std::string ver = Platform::GetOSVersion();
    std::cout << "os-version: " << ver << std::endl;

    EXPECT_TRUE(ver.size());
}
