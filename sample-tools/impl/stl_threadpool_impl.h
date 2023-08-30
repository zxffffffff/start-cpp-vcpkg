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

/* 兼容 windows */
#undef min
#undef max

/* 线程池容量，不弹性扩容 */
class ThreadPool
{
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;

    std::mutex queueMutex;
    std::condition_variable condition;

    std::atomic<bool> stop{false};
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
                               { return stop || !tasks.empty(); });
                if (stop && tasks.empty())
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
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            stop = true;
        }

        condition.notify_all();

        for (std::thread &thread : threads)
        {
            thread.join();
        }
    }

    void MoveToThread(std::function<void()> task)
    {
        assert(!stop);
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            tasks.push(std::move(task));
        }
        condition.notify_one();
    }

    int GetIdleThreadCount() const
    {
        return idleThreads.load();
    }
};

/* 临时线程管理 */
class ThreadMgr
{
    std::mutex mutex;
    std::list<std::thread> threads;

    std::atomic<bool> stop{false};

private:
    void workerThread(std::function<void()> task)
    {
        task();

        std::unique_lock<std::mutex> lock(mutex);
        auto it = std::find_if(threads.begin(), threads.end(), [](const std::thread& t) {
            return t.get_id() == std::this_thread::get_id();
        });
        if (it != threads.end()) {
            it->detach();
            threads.erase(it);
        }
    }

public:
    ~ThreadMgr()
    {
        stop = true;

        std::list<std::thread> threads2;
        {
            std::unique_lock<std::mutex> lock(mutex);
            threads2.swap(threads);
        }

        for (std::thread &thread : threads2)
        {
            thread.join();
        }
    }

    void MoveToThread(std::function<void()> task)
    {
        assert(!stop);
        std::unique_lock<std::mutex> lock(mutex);
        threads.emplace_back(&ThreadMgr::workerThread, this, std::move(task));
    }
};

template <int count>
class ThreadPoolImpl : public IThreadPool
{
    std::unique_ptr<ThreadPool> threadPool = std::make_unique<ThreadPool>(count);
    std::unique_ptr<ThreadMgr> threadMgr = std::make_unique<ThreadMgr>();

public:
    virtual std::future<void> MoveToThread(std::function<void()> f) override
    {
        auto promise = std::make_shared<std::promise<void>>();
        auto future = promise->get_future();
        auto task = [=]
        {
            f();
            promise->set_value();
        };
        
        if (threadPool->GetIdleThreadCount() > 0)
            threadPool->MoveToThread(task);
        else
            threadMgr->MoveToThread(task);
        return future;
    }
};
