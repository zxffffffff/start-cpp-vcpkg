/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#include <gtest/gtest.h>
#include "impl/boost_threadpool_impl.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable : 4566)
#endif

TEST(boost_threadpool_impl, Test)
{
    /* 不自动扩容 */
    auto pool = std::make_unique<ThreadPoolImpl>(4);
    static std::atomic<int> flag{0};

    for (int i = 0; i < 4 * 10; ++i)
    {
        auto test = []
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            ++flag;
        };
        pool->MoveToThread(test);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100 * 10 / 2 + 50));
    EXPECT_GE(flag.load(), 4 * 10 / 2);
    std::this_thread::sleep_for(std::chrono::milliseconds(100 * 10 / 2 + 50));
    ASSERT_EQ(flag.load(), 4 * 10);
}

TEST(boost_threadpool_impl, Recursive)
{
    /* 不自动扩容 */
    auto pool = std::make_unique<ThreadPoolImpl>(4);
    static std::atomic<int> flag{0};

    for (int i = 0; i < 4 * 5; ++i)
    {
        auto test = [&]
        {
            // i * 2
            {
                auto test2 = [&]
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    ++flag;
                };
                pool->MoveToThread(test2);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            ++flag;
        };
        pool->MoveToThread(test);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100 * 10 / 2 + 50));
    EXPECT_GE(flag.load(), 4 * 10 / 2);
    std::this_thread::sleep_for(std::chrono::milliseconds(100 * 10 / 2 + 50));
    ASSERT_EQ(flag.load(), 4 * 10);
}

TEST(boost_threadpool_impl, Shutdown)
{
    /* 不自动扩容 */
    auto pool = std::make_unique<ThreadPoolImpl>(4);
    static std::atomic<int> flag{0};

    for (int i = 0; i < 4 * 10; ++i)
    {
        auto test = []
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            ++flag;
        };
        pool->MoveToThread(test);
    }

    while (flag < 10)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    pool = nullptr; // Shutdown
}
