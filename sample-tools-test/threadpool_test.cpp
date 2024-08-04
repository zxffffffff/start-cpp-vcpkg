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
    /* 自动扩容 */
    auto pool = std::make_unique<ThreadPoolImpl>(4);
    static std::atomic<int> flag{0};

    int cpus = Hardware::GetCPUs();
    cpus += cpus % 2;
    int sleep_ms = 1000 / cpus;
    sleep_ms = std::min(std::max(100, sleep_ms), 1000);

    for (int i = 0; i < 4 * cpus; ++i)
    {
        auto test = [sleep_ms]
        {
            Common::Sleep(sleep_ms);
            ++flag;
        };
        pool->MoveToThread(test);
    }

    /* 不准确，受到电脑性能影响 */
    Common::Sleep(sleep_ms * cpus / 2 + sleep_ms / 2);
    EXPECT_GE(flag.load(), 4 * cpus / 2);
    Common::Sleep(sleep_ms * cpus / 2 + sleep_ms / 2);
    ASSERT_EQ(flag.load(), 4 * cpus);
}

/* 警告：Google Test 仅在 *nix 上线程安全，Windows 或其他平台不支持多线程断言 */
TEST(stl_threadpool_impl, Recursive)
{
    /* 自动扩容 */
    auto pool = std::make_unique<ThreadPoolImpl>(4);
    auto p_pool = pool.get();
    static std::atomic<int> flag{0};

    int cpus = Hardware::GetCPUs();
    cpus += cpus % 2;
    int sleep_ms = 1000 / cpus;
    sleep_ms = std::min(std::max(100, sleep_ms), 1000);

    for (int i = 0; i < 4 * cpus; ++i)
    {
        auto test = [=]
        {
            // i * 2
            {
                auto test2 = [=]
                {
                    Common::Sleep(sleep_ms);
                    ++flag;
                    return true;
                };
                p_pool->MoveToThread(test2);
            }
            Common::Sleep(sleep_ms);
            ++flag;
            return true;
        };
        pool->MoveToThread(test);
    }

    /* 不准确，受到电脑性能影响 */
    Common::Sleep(sleep_ms * cpus / 2 + sleep_ms / 2);
    EXPECT_GE(flag.load(), 4 * cpus / 2);
    Common::Sleep(sleep_ms * cpus / 2 + sleep_ms / 2);
    ASSERT_EQ(flag.load(), 4 * cpus);
}

/* 警告：Google Test 仅在 *nix 上线程安全，Windows 或其他平台不支持多线程断言 */
TEST(stl_threadpool_impl, Shutdown)
{
    /* 不自动扩容 */
    auto pool = std::make_unique<ThreadPoolImpl>(4);
    static std::atomic<int> flag{0};

    int cpus = Hardware::GetCPUs();
    cpus += cpus % 2;
    int sleep_ms = 1000 / cpus;
    sleep_ms = std::min(std::max(100, sleep_ms), 1000);

    for (int i = 0; i < 4 * cpus; ++i)
    {
        auto test = [sleep_ms]
        {
            Common::Sleep(sleep_ms);
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
