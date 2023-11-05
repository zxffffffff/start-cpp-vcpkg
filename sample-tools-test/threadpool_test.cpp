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
#pragma warning(disable:4566)
#endif

TEST(ThreadPoolImpl, Test)
{
    auto pool = std::make_shared<ThreadPoolImpl<4>>();
    static std::atomic<int> flag{0};

    for (int i = 0; i < 8; ++i) 
    {
        auto test = [pool, i]
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10 * i));
            ++flag;
            return true;
        };
        pool->MoveToThread(test);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    ASSERT_GT(flag, 4);

    pool = nullptr;
}

TEST(ThreadPoolImpl, Recursive)
{
    auto pool = std::make_shared<ThreadPoolImpl<4>>();
    static std::atomic<int> flag{0};

    for (int i = 0; i < 8; ++i)
    {
        auto test = [pool, i]
        {
            for (int j = 0; j < 8; ++j)
            {
                auto test2 = [pool, j]
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10 * j));
                    ++flag;
                    return true;
                };
                pool->MoveToThread(test2);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10 * i));
            ++flag;
            return true;
        };
        pool->MoveToThread(test);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    ASSERT_GT(flag, 8);

    pool = nullptr;
}
