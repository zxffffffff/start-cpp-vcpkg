/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#include <gtest/gtest.h>
#include "common.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable : 4566)
#endif

TEST(guid, test)
{
    std::set<std::string> guids;
    const int cnt = 1000;
    for (int i = 0; i < cnt; ++i)
    {
        guids.insert(Common::GenGuid());
    }
    ASSERT_EQ(guids.size(), cnt);
}
