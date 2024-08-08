/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#include <gtest/gtest.h>
#include "common.h"
#include <thread>
#include <sstream>

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

TEST(Common, Guid)
{
    std::string guid = Common::GenGuid();
    std::string guid2 = Common::GenGuid();
    ASSERT_FALSE(guid.empty());
    ASSERT_NE(guid, guid2);

    std::set<std::string> guids;
    const int cnt = 100000;
    for (int i = 0; i < cnt; ++i)
    {
        guids.insert(Common::GenGuid());
    }
    ASSERT_EQ(guids.size(), cnt);
}

TEST(Common, Guid2)
{
    std::string guid = Common::GenGuid2();
    std::string guid2 = Common::GenGuid2();
    ASSERT_FALSE(guid.empty());
    ASSERT_NE(guid, guid2);

    std::set<std::string> guids;
    const int cnt = 100000;
    for (int i = 0; i < cnt; ++i)
    {
        guids.insert(Common::GenGuid2());
    }
    ASSERT_EQ(guids.size(), cnt);
}

TEST(Common, Random)
{
    std::set<int> nums;
    for (int i = 0; i < 100 * 10000; ++i)
    {
        nums.insert(Common::Random());
    }
    /* 会有重复，尽可能的随机就行 */
    EXPECT_GT(nums.size(), 99 * 10000);
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
