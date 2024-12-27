/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#include <gtest/gtest.h>
#include "hardware.h"
#include <thread>

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable : 4566)
#endif

TEST(cpu, test)
{
    int cpus = Hardware::GetCPUs();

    std::cout << "cpu: " << cpus << std::endl;
    ASSERT_GT(cpus, 0);
}

TEST(macAddr, test)
{
    std::string mac = Hardware::GetMacAddr();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::string mac2 = Hardware::GetMacAddr();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::string mac3 = Hardware::GetMacAddr();

    std::cout << "mac: " << mac << std::endl;
    EXPECT_TRUE(mac.size());
    EXPECT_EQ(mac, mac2);
    EXPECT_EQ(mac, mac3);
}
