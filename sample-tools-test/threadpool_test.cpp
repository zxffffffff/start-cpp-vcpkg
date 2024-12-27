/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#include <gtest/gtest.h>
#include "impl/stl_threadpool_impl.h"
#include "common.h"
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
TEST(stl_threadpool_impl, Test)
{
    /* 低端设备无法准确计时 */
    if (Hardware::GetCPUs() < 8)
        return;

    /* 自动扩容 */
    auto pool = std::make_unique<ThreadPoolImpl>(4);
    static std::atomic<int> flag{0};

    for (int i = 0; i < 4 * 10; ++i)
    {
        auto test = []
        {
            Common::Sleep(100);
            ++flag;
        };
        pool->MoveToThread(test);
    }

    /* 不准确，受到电脑性能影响 */
    Common::Sleep(100 * 10 / 2 + 100 / 2);
    EXPECT_GE(flag.load(), 4 * 10 / 2);
    Common::Sleep(100 * 10 / 2 + 100 / 2);
    ASSERT_EQ(flag.load(), 4 * 10);
}

/* 警告：Google Test 仅在 *nix 上线程安全，Windows 或其他平台不支持多线程断言 */
TEST(stl_threadpool_impl, Recursive)
{
    /* 低端设备无法准确计时 */
    if (Hardware::GetCPUs() < 8)
        return;

    /* 自动扩容 */
    auto pool = std::make_unique<ThreadPoolImpl>(4);
    auto p_pool = pool.get();
    static std::atomic<int> flag{0};

    for (int i = 0; i < 4 * 10; ++i)
    {
        auto test = [=]
        {
            // i * 2
            {
                auto test2 = [=]
                {
                    Common::Sleep(100);
                    ++flag;
                    return true;
                };
                p_pool->MoveToThread(test2);
            }
            Common::Sleep(100);
            ++flag;
            return true;
        };
        pool->MoveToThread(test);
    }

    /* 不准确，受到电脑性能影响 */
    Common::Sleep(100 * 10 / 2 + 100 / 2);
    EXPECT_GE(flag.load(), 4 * 10 / 2);
    Common::Sleep(100 * 10 / 2 + 100 / 2);
    ASSERT_EQ(flag.load(), 4 * 10);
}

/* 警告：Google Test 仅在 *nix 上线程安全，Windows 或其他平台不支持多线程断言 */
TEST(stl_threadpool_impl, Shutdown)
{
    /* 低端设备无法准确计时 */
    if (Hardware::GetCPUs() < 8)
        return;

    /* 不自动扩容 */
    auto pool = std::make_unique<ThreadPoolImpl>(4);
    static std::atomic<int> flag{0};

    for (int i = 0; i < 4 * 10; ++i)
    {
        auto test = []
        {
            Common::Sleep(100);
            ++flag;
        };
        pool->MoveToThread(test);
    }

    while (flag < 10)
    {
        Common::Sleep(10);
    }
    pool = nullptr; // Shutdown
}
