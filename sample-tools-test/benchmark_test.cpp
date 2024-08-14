/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#include <gtest/gtest.h>
#include "benchmark.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable : 4566)
#endif

TEST(benchmark, once)
{
    for (int i = 0; i < Hardware::GetCPUs(); ++i)
    {
        double use_ms = Benchmark::RunOnce();
        double score = Benchmark::Score(use_ms);

        std::cout << "benchmark score: " << score << std::endl;
        EXPECT_NE(score, 0);
    }
}

TEST(benchmark, multi_thread)
{
    double score = Benchmark::RunMultiThread(3);

    std::cout << "benchmark multi_thread score: " << score << std::endl;
    EXPECT_NE(score, 0);
}
