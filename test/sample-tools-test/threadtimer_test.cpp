/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#include <gtest/gtest.h>
#include "impl/stl_threadtimer_impl.h"

TEST(ThreadTimerImpl, Test)
{
    static std::atomic_int flag = 0;
    ThreadTimerImpl timer(50, [] { ++flag; });
    timer.Start();

    std::this_thread::sleep_for(std::chrono::milliseconds(75));
    EXPECT_EQ(flag, 1);

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_EQ(flag, 2);

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_EQ(flag, 3);
}

TEST(ThreadTimerImpl, Recursive)
{
    static std::atomic_int flag = 0;

    auto recursive = []
    {
        auto timer = new ThreadTimerImpl(50, [] { ++flag; });
        timer->Start();
    };

    ThreadTimerImpl timer(50, [&] { ++flag; recursive(); });
    timer.Start();

    std::this_thread::sleep_for(std::chrono::milliseconds(75));
    EXPECT_EQ(flag, 0 + 1 + 0);

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_EQ(flag, 1 + 1 + 1 + 0);

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_EQ(flag, 3 + 1 + 1 + 1 + 0);

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_EQ(flag, 6 + 1 + 1 + 1 + 1 + 0);
}
