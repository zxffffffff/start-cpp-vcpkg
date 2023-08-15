/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include "../threadtimer_interface.h"
#include <thread>
#include <future>

class ThreadTimerImpl : public IThreadTimer
{
    struct ThreadData
    {
        std::function<void()> m_callback;
        std::atomic_int m_interval_ms{ 50 };
        std::atomic_bool m_running{ false };
    };
    std::unique_ptr<ThreadData> thread_data = std::make_unique<ThreadData>();

    std::thread m_thread;

public:
    ThreadTimerImpl() {}
    ThreadTimerImpl(int ms, std::function<void()> f)
    {
        SetInterval(ms);
        SetCbk(f);
    }
    virtual ~ThreadTimerImpl() 
    {
        Stop();
    }

    virtual void SetInterval(int ms) override
    {
        thread_data->m_interval_ms = ms;
    }

    virtual int  GetInterval() const override
    {
        return thread_data->m_interval_ms;
    }

    virtual void SetCbk(std::function<void()> f)override
    {
        thread_data->m_callback = f;
    }

    virtual void Start() override
    {
        if (thread_data->m_running)
            return;
        thread_data->m_running = true;

        //LOG(INFO) << __FUNCTION__ << " interval=" << thread_data->m_interval_ms;
        m_thread = std::thread(&ThreadTimerImpl::run, thread_data.get());
    }

    virtual void Start(int ms) override
    {
        SetInterval(ms);
        Start();
    }

    virtual void Start(int ms, std::function<void()> f)override
    {
        SetInterval(ms);
        SetCbk(f);
        Start();
    }

    virtual void Stop() override
    {
        if (!thread_data->m_running)
            return;
        thread_data->m_running = false;

        //LOG(INFO) << __func__;
        if (m_thread.joinable())
        {
            m_thread.join();
        }
    }

    virtual bool IsRunning() const override
    {
        return thread_data->m_running;
    }

private:
    static void run(ThreadData* thread_data)
    {
        size_t count = 0;
        while (thread_data->m_running)
        {
            auto start = std::chrono::steady_clock::now();
            std::chrono::milliseconds duration(thread_data->m_interval_ms);
            while (thread_data->m_running && std::chrono::steady_clock::now() - start < duration)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }

            if (thread_data->m_running)
            {
                if (thread_data->m_callback)
                {
                    //DLOG(INFO) << __func__
                    //    << " interval=" << thread_data->m_interval_ms
                    //    << " count=" << ++count;

                    thread_data->m_callback();
                }
                else
                {
                    //DLOG(WARNING) << __func__
                    //    << " interval=" << thread_data->m_interval_ms
                    //    << " count=" << ++count
                    //    << " no callback !";
                }
            }
        }
    }
};
