/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#include <gtest/gtest.h>
#include "common.h"
#include "cpp_def.h"
#include <thread>
#include <sstream>
#include <unordered_set>

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable : 4566)
#endif

TEST(Common, Time)
{
    auto start = Common::NowSinceEpoch_MS();
    ASSERT_GT(start, 0);

    Common::Sleep(100);
    auto now = Common::NowSinceEpoch_MS();
    ASSERT_GE(now, start + 100);

    std::string s = Common::TimeFormat(start);
    ASSERT_NE(s.find("-"), std::string::npos);
    ASSERT_NE(s.find(" "), std::string::npos);
    ASSERT_NE(s.find(":"), std::string::npos);
    ASSERT_NE(s.find("."), std::string::npos);

    std::string s2 = Common::TimeFormat(now);
    ASSERT_LT(s, s2);
}

TEST(Common, GenUuid)
{
    std::string uuid = Common::GenUuid();
    std::string uuid2 = Common::GenUuid();
    std::cout << VAR_NAME(uuid) << ": " << uuid << std::endl;
    std::cout << VAR_NAME(uuid2) << ": " << uuid2 << std::endl;
    ASSERT_FALSE(uuid.empty());
    ASSERT_NE(uuid, uuid2);

    std::unordered_set<std::string> uuids;
    const int cnt = 1000 * 1000;
    for (int i = 0; i < cnt; ++i)
    {
        std::string uuid3 = Common::GenUuid();
        if (i == 0)
            std::cout << VAR_NAME(uuid3) << ": " << uuid3 << std::endl;
        uuids.insert(uuid3);
    }
    ASSERT_EQ(uuids.size(), cnt);
}

TEST(Common, GenUuid2)
{
    std::string uuid = Common::GenUuid2();
    std::string uuid2 = Common::GenUuid2();
    std::cout << VAR_NAME(uuid) << ": " << uuid << std::endl;
    std::cout << VAR_NAME(uuid2) << ": " << uuid2 << std::endl;
    ASSERT_FALSE(uuid.empty());
    ASSERT_NE(uuid, uuid2);

    std::unordered_set<std::string> uuids;
    const int cnt = 1000 * 1000;
    for (int i = 0; i < cnt; ++i)
    {
        std::string uuid3 = Common::GenUuid2();
        if (i == 0)
            std::cout << VAR_NAME(uuid3) << ": " << uuid3 << std::endl;
        uuids.insert(uuid3);
    }
    EXPECT_GT(uuids.size(), cnt * 0.999);
}

TEST(Common, Random)
{
    std::unordered_set<int> nums;
    const int cnt = 1000 * 1000;
    for (int i = 0; i < cnt; ++i)
    {
        nums.insert(Common::Random());
    }
    /* 会有重复，尽可能的随机就行 */
    EXPECT_GT(nums.size(), cnt * 0.999);
}

TEST(Common, toThreadLocalStr)
{
    auto test = [](int i)
    {
        for (int j = 0; j < 10000; ++j)
        {
            std::stringstream ss;
            ss << std::this_thread::get_id() << "-" << i << "-" << j;
            std::string s = ss.str();
            const char *p = Common::toThreadLocalStr(s);
            ASSERT_EQ(s, std::string(p));
            // std::cerr << s << std::endl;
        }
    };

    std::vector<std::thread> v;
    for (int i = 0; i < 100; ++i)
    {
        v.emplace_back(test, i);
    }
    for (auto &th : v)
    {
        if (th.joinable())
            th.join();
    }
}
