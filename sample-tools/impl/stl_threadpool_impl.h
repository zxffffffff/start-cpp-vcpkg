/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include "../interface/threadpool_interface.h"
#include "progschj-ThreadPool.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable : 4566)
#endif

class ThreadPoolImpl : public IThreadPool
{
    std::unique_ptr<progschj::ThreadPool> pool;
    std::atomic_bool m_running{true};

public:
    ThreadPoolImpl(int count)
        : IThreadPool(count), pool(std::make_unique<progschj::ThreadPool>(count))
    {
    }

    virtual ~ThreadPoolImpl()
    {
        m_running = false;
        pool->wait_until_empty();
        pool->wait_until_nothing_in_flight();
    }

    virtual void Resize(int count) override
    {
        if (!m_running)
            return;
        pool->set_pool_size(count);
    }

    virtual void MoveToThread(std::function<void()> f) override
    {
        if (!m_running)
            return;
        pool->enqueue(f);
    }
};
