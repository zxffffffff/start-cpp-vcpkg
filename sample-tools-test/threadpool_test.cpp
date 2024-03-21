/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#include <gtest/gtest.h>
#include "impl/stl_threadpool_impl.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable : 4566)
#endif

TEST(stl_threadpool_impl, Test)
{
    /* 自动扩容 */
    auto pool = std::make_shared<ThreadPoolImpl>(4);
    static std::atomic<int> flag{0};

    for (int i = 0; i < 4 * 10; ++i)
    {
        auto test = [pool, i]
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            ++flag;
        };
        pool->MoveToThread(test);
    }

    /* 不准确，受到电脑性能影响 */
    std::this_thread::sleep_for(std::chrono::milliseconds(100 * 10 / 2 + 50));
    EXPECT_GT(flag.load(), 4 * 10 / 2);
    std::this_thread::sleep_for(std::chrono::milliseconds(100 * 10 / 2 + 50));
    ASSERT_EQ(flag.load(), 4 * 10);
}

TEST(stl_threadpool_impl, Recursive)
{
    /* 自动扩容 */
    auto pool = std::make_shared<ThreadPoolImpl>(4);
    static std::atomic<int> flag{0};

    for (int i = 0; i < 4 * 5; ++i)
    {
        auto test = [pool, i]
        {
            // i * 2
            {
                auto test2 = [pool]
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    ++flag;
                    return true;
                };
                pool->MoveToThread(test2);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            ++flag;
            return true;
        };
        pool->MoveToThread(test);
    }

    /* 不准确，受到电脑性能影响 */
    std::this_thread::sleep_for(std::chrono::milliseconds(100 * 10 / 2 + 50));
    EXPECT_GT(flag.load(), 4 * 10 / 2);
    std::this_thread::sleep_for(std::chrono::milliseconds(100 * 10 / 2 + 50));
    ASSERT_EQ(flag.load(), 4 * 10);
}
