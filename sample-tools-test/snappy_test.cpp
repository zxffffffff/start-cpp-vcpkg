/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#include "gtest/gtest.h"
#include "impl/snappy_impl.h"
#include "common.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable : 4566)
#endif

TEST(CompressionTest, Snappy_version)
{
    std::stringstream ss;
    ss << SNAPPY_MAJOR << "." << SNAPPY_MINOR << "." << SNAPPY_PATCHLEVEL;

    std::string ver = ss.str();
    ASSERT_FALSE(ver.empty());

    std::cout << "snappy version: " << ver << std::endl;
}

TEST(CompressionTest, Snappy_Impl)
{
    Snappy_Impl obj;

    std::string msg = "abc123!@#$%^&*()_-+=*/\\'\"?你好";
    std::string res = obj.Compress(msg);
    EXPECT_NE(msg, res);
    std::string msg2 = obj.Uncompress(res);
    ASSERT_EQ(msg, msg2);
}
