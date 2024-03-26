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

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable : 4566)
#endif

/* 线程池，不支持弹性扩容 */
class ThreadPoolImpl : public IThreadPool
{
    boost::asio::thread_pool pool;
    std::atomic_bool m_running{true};

public:
    ThreadPoolImpl(int count)
        : IThreadPool(count), pool(count)
    {
    }

    virtual ~ThreadPoolImpl()
    {
        m_running = false;
        pool.join();
    }

    virtual void MoveToThread(std::function<void()> f) override
    {
        if (!m_running)
            return;
        boost::asio::post(pool, f);
    }
};
