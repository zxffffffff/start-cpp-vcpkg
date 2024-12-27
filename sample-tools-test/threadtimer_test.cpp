/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#include <gtest/gtest.h>
#include "impl/stl_threadtimer_impl.h"
#include "platform.h"
#include "hardware.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable : 4566)
#endif

/* 警告：Google Test 仅在 *nix 上线程安全，Windows 或其他平台不支持多线程断言 */
TEST(ThreadTimerImpl, Test)
{
    /* 低端设备无法准确计时 */
    if (Hardware::GetCPUs() < 8)
        return;

    static std::atomic<int> flag{0};
    ThreadTimerImpl timer(100, []
                          { ++flag; });
    timer.Start();

    std::this_thread::sleep_for(std::chrono::milliseconds(100 + 100 / 2));
    EXPECT_GE(flag.load(), 1);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_GE(flag.load(), 2);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_GE(flag.load(), 3);
}

/* 警告：Google Test 仅在 *nix 上线程安全，Windows 或其他平台不支持多线程断言 */
TEST(ThreadTimerImpl, Recursive)
{
    /* 低端设备无法准确计时 */
    if (Hardware::GetCPUs() < 8)
        return;

    static std::atomic<int> flag{0};

    auto recursive = []
    {
        auto timer = new ThreadTimerImpl(100, []
                                         { ++flag; });
        timer->Start();
    };

    ThreadTimerImpl timer(100, [=]
                          { ++flag; recursive(); });
    timer.Start();

    std::this_thread::sleep_for(std::chrono::milliseconds(100 + 100 / 2));
    EXPECT_GE(flag.load(), 0 + 1 + 0);

    /* 不准确，受到电脑性能影响 */
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_GE(flag.load(), 1 + 1 + 1 + 0);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_GE(flag.load(), 3 + 1 + 1 + 1 + 0);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_GE(flag.load(), 6 + 1 + 1 + 1 + 1 + 0);
}
