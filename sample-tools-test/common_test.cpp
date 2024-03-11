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

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    auto now = Common::NowSinceEpoch_MS();
    ASSERT_GE(now, start + 100);
}

TEST(Common, Guid)
{
    std::string guid = Common::GenGuid();
    std::string guid2 = Common::GenGuid();
    ASSERT_FALSE(guid.empty());
    ASSERT_NE(guid, guid2);

    std::set<std::string> guids;
    const int cnt = 1000;
    for (int i = 0; i < cnt; ++i)
    {
        guids.insert(Common::GenGuid());
    }
    ASSERT_EQ(guids.size(), cnt);
}
