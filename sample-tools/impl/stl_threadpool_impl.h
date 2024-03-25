/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include "../interface/threadpool_interface.h"
#include <queue>
#include <list>
#include <atomic>
#include <mutex>
#include <thread>
#include <future>
#include <memory>

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable : 4566)
#endif

/* [1]线程池，不支持弹性扩容 */
class ThreadPool
{
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;

    std::mutex queueMutex;
    std::condition_variable condition;

    std::atomic_bool m_running{true};
    std::atomic<int> idleThreads{0};

private:
    void workerThread()
    {
        while (true)
        {
            std::function<void()> task;
            ++idleThreads;
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                condition.wait(lock, [this]
                               { return !m_running || !tasks.empty(); });
                if (!m_running && tasks.empty())
                    return;

                task = std::move(tasks.front());
                tasks.pop();
            }
            --idleThreads;
            task();
        }
    }

public:
    ThreadPool(int threadCount)
    {
        for (int i = 0; i < threadCount; ++i)
        {
            threads.emplace_back(&ThreadPool::workerThread, this);
        }
    }

    ~ThreadPool()
    {
        m_running = false;
        condition.notify_all();

        for (std::thread &thread : threads)
        {
            if (thread.joinable())
                thread.join();
        }
    }

    bool MoveToThread(std::function<void()> task)
    {
        if (!m_running)
            return false;

        {
            std::unique_lock<std::mutex> lock(queueMutex);
            tasks.push(std::move(task));
        }
        condition.notify_one();
        return true;
    }

    int GetIdleThreadCount() const
    {
        if (!m_running)
            return 0;
        return idleThreads.load();
    }
};

/* [2]临时线程 */
class ThreadMgr
{
    std::mutex mutex;
    std::list<std::thread> threads;

    std::atomic_bool m_running{true};

private:
    void workerThread(std::function<void()> task)
    {
        task();

        std::unique_lock<std::mutex> lock(mutex);
        auto it = std::find_if(threads.begin(), threads.end(), [](const std::thread &t)
                               { return t.get_id() == std::this_thread::get_id(); });
        if (it != threads.end())
        {
            it->detach();
            threads.erase(it);
        }
    }

public:
    ~ThreadMgr()
    {
        m_running = false;

        std::list<std::thread> threads2;
        {
            std::unique_lock<std::mutex> lock(mutex);
            threads2.swap(threads);
        }

        for (std::thread &thread : threads2)
        {
            if (thread.joinable())
                thread.join();
        }
    }

    bool MoveToThread(std::function<void()> task)
    {
        if (!m_running)
            return false;

        std::unique_lock<std::mutex> lock(mutex);
        threads.emplace_back(&ThreadMgr::workerThread, this, std::move(task));
        return true;
    }
};

/* [1]+[2]组合，支持弹性扩容 */
class ThreadPoolImpl : public IThreadPool
{
    std::unique_ptr<ThreadPool> threadPool;
    std::unique_ptr<ThreadMgr> threadMgr;

public:
    ThreadPoolImpl(int count)
        : IThreadPool(count), threadPool(std::make_unique<ThreadPool>(count)), threadMgr(std::make_unique<ThreadMgr>())
    {
    }

    virtual std::future<bool> MoveToThread(std::function<void()> f) override
    {
        auto promise = std::make_shared<std::promise<bool>>();
        auto future = promise->get_future();
        auto task = [=]
        {
            f();
            promise->set_value(true);
        };

        bool ret = false;
        if (threadPool->GetIdleThreadCount() > 0)
            ret = threadPool->MoveToThread(task);
        else
            ret = threadMgr->MoveToThread(task);

        if (!ret)
            promise->set_value(false);
        return future;
    }
};
