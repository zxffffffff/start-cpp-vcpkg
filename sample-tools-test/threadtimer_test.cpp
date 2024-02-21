/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#include <gtest/gtest.h>
#include "impl/stl_threadtimer_impl.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable : 4566)
#endif

TEST(ThreadTimerImpl, Test)
{
    static std::atomic<int> flag{0};
    ThreadTimerImpl timer(50, []
                          { ++flag; });
    timer.Start();

    /* 确保timer启动 */
    while (flag == 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    flag = 0;
    std::this_thread::sleep_for(std::chrono::milliseconds(25));

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_EQ(flag, 1);

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_EQ(flag, 2);

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_EQ(flag, 3);
}

TEST(ThreadTimerImpl, Recursive)
{
    static std::atomic<int> flag{0};

    auto recursive = []
    {
        auto timer = new ThreadTimerImpl(50, []
                                         { ++flag; });
        timer->Start();
    };

    ThreadTimerImpl timer(50, [&]
                          { ++flag; recursive(); });
    timer.Start();

    std::this_thread::sleep_for(std::chrono::milliseconds(75));
    EXPECT_EQ(flag, 0 + 1 + 0);

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_EQ(flag, 1 + 1 + 1 + 0);

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_EQ(flag, 3 + 1 + 1 + 1 + 0);

    // std::this_thread::sleep_for(std::chrono::milliseconds(50));
    // EXPECT_EQ(flag, 6 + 1 + 1 + 1 + 1 + 0);
}
