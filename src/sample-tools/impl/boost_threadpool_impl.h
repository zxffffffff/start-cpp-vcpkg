/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include "../interface/threadpool_interface.h"
#include <boost/asio.hpp>

/* 数量不足时，会自动创建临时线程 */
template <int count>
class ThreadPoolImpl : public IThreadPool
{
    boost::asio::thread_pool pool;

public:
    ThreadPoolImpl()
        : pool(count)
    {
    }

    virtual ~ThreadPoolImpl()
    {
        /* 等待所有任务完成 */
        pool.join();
    }

    virtual std::future<void> MoveToThread(std::function<void()> f) override
    {
        auto promise = std::make_shared<std::promise<void>>();
        auto future = promise->get_future();
        boost::asio::post(pool, [=]
                          { f(); promise->set_value(); });
        return future;
    }
};
