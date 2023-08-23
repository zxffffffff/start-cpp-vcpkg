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
#include <atomic>
#include <mutex>
#include <thread>
#include <future>

/* 兼容 windows */
#undef min
#undef max

/* 线程池最大容量,应尽量设小一点 */
template <int maxCount>
class ThreadPool
{
    std::vector<std::thread> pool;                    /* 线程池 */
    std::queue<std::packaged_task<void()>> taskQueue; /* 任务队列 */
    std::mutex mutex;                                 /* 同步 */
    std::condition_variable cv;                       /* 条件阻塞 */
    std::atomic_bool running{true};                   /* 线程池是否执行 */
    std::atomic_int idlCount{0};                      /* 空闲线程数量 */

public:
    ThreadPool()
    {
        AddThread(2);

        // LOG(INFO) << __func__
        //     << " MaxCount=" << MaxCount()
        //     << " TotCount=" << TotCount()
        //     << " IdlCount=" << IdlCount();
    }

    virtual ~ThreadPool()
    {
        // LOG(INFO) << __func__
        //     << " MaxCount=" << MaxCount()
        //     << " TotCount=" << TotCount()
        //     << " IdlCount=" << IdlCount();

        running = false;
        cv.notify_all(); /* 唤醒所有线程执行 */
        for (std::thread &thread : pool)
        {
            // thread.detach(); /* 让线程“自生自灭” */
            if (thread.joinable())
                thread.join(); /* 等待任务结束， 前提：线程一定会执行完 */
        }
    }

    /* 最大线程数量 */
    int MaxCount() const { return maxCount; }

    /* 线程数量 */
    int TotCount() const { return pool.size(); }

    /* 空闲线程数量 */
    int IdlCount() const { return idlCount; }

    /* [1] 添加指定数量的线程（构造时默认添加_idlThrNum） */
    void AddThread(unsigned short size)
    {
        // LOG(INFO) << __func__
        //     << " MaxCount=" << MaxCount()
        //     << " TotCount=" << TotCount()
        //     << " IdlCount=" << IdlCount()
        //     << " addSize=" << size;

        for (; size > 0; --size)
        {
            auto worker = [this]
            {
                ++idlCount;
                while (running)
                {
                    std::packaged_task<void()> task;
                    {
                        std::unique_lock<std::mutex> lock{mutex};
                        cv.wait(lock, [this]
                                { return !running || !taskQueue.empty(); }); /* 等待任务队列不为空 */
                        if (!running && taskQueue.empty())
                            return;
                        task = std::move(taskQueue.front()); /* 取出队列中的任务 */
                        taskQueue.pop();
                    }
                    --idlCount;
                    task(); /* 执行任务 */
                    ++idlCount;

                    if (pool.size() > maxCount) /* 临时的线程 */
                        break;
                }
                --idlCount;
            };
            pool.emplace_back(worker);
        }
    }

    /* [2] 添加任务 */
    template <class F, class... Args>
    std::future<void> MoveToThread(F &&f, Args &&...args)
    {
        if (!running) // stoped ?
            throw std::runtime_error(__func__ + std::string(" error: is stopped!"));

        std::packaged_task<void()> task(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        std::future<void> future = task.get_future();

        {
            /* 添加任务到队列 */
            std::lock_guard<std::mutex> lock{mutex};
            taskQueue.push(std::move(task));
            if (idlCount < 1)
                AddThread(1);
        }
        cv.notify_one(); /* 唤醒一个线程执行 */

        return future;
    }
};

template <int count>
class ThreadPoolImpl : public IThreadPool
{
    ThreadPool<count> threadPool;

public:
    virtual std::future<void> MoveToThread(std::function<void()> f) override
    {
        auto promise = std::make_shared<std::promise<void>>();
        auto future = promise->get_future();
        threadPool.MoveToThread([=]
                                { f(); promise->set_value(); });
        return future;
    }
};
