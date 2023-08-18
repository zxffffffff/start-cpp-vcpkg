/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#include <gtest/gtest.h>
#include "impl/stl_threadpool_impl.h"

TEST(ThreadPoolImpl, Test)
{
    static ThreadPoolImpl pool;
    static std::atomic_int flag = 0;

    for (int i = 0; i < 8; ++i) 
    {
        auto test = [i]
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10 * i));
            ++flag;
            return true;
        };
        pool.MoveToThread(test);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    ASSERT_EQ(flag, 8);
}

TEST(ThreadPoolImpl, Recursive)
{
    static ThreadPoolImpl pool;
    static std::atomic_int flag = 0;

    for (int i = 0; i < 8; ++i)
    {
        auto test = [i]
        {
            for (int j = 0; j < 8; ++j)
            {
                auto test2 = [j]
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10 * j));
                    ++flag;
                    return true;
                };
                pool.MoveToThread(test2);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10 * i));
            ++flag;
            return true;
        };
        pool.MoveToThread(test);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    ASSERT_EQ(flag, 8 * 8 + 8);
}
