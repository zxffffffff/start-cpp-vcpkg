/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include "../interface/tcp_interface.h"
#include <uv.h>
#include <mutex>
#include <list>
#include <cassert>

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable : 4566)
#endif

inline Error MakeStatusError(int state, int uv_status)
{
    std::stringstream ss;
    ss << uv_err_name(uv_status) << ":" << uv_strerror(uv_status);
    return MakeError(state, ss.str());
}

class EventLoop
{
private:
    struct ThreadData
    {
        std::atomic_bool is_running{false};
        std::thread::id this_thread_id;

        uv_loop_t loop{0};
        uv_async_t async{0};

        std::mutex async_mutex;
        std::list<std::function<void()>> async_cbk;
    };
    std::unique_ptr<ThreadData> thread_data = std::make_unique<ThreadData>();

    std::atomic_bool is_start{false};
    uv_thread_t thread_id = 0;

public:
    /* 阻塞：等待线程运行 */
    void start()
    {
        if (is_start)
        {
            /* 必须按顺序调用 */
            assert(false);
            return;
        }
        is_start = true;

        if (thread_id)
        {
            assert(false);
            return;
        }

        uv_thread_create(&thread_id, run, thread_data.get());
        while (!thread_data->is_running)
        {
            // wait for init
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    /* 阻塞：等待线程结束 */
    void stop()
    {
        if (!is_start)
        {
            /* 必须按顺序调用 */
            assert(false);
            return;
        }
        is_start = false;

        if (!thread_id)
        {
            assert(false);
            return;
        }

        moveToThread([this]
                     { uv_stop(&thread_data->loop); });
        while (thread_data->is_running)
        {
            // wait for close
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        thread_id = 0; // uv_thread_join(&thread_id);
    }

    uv_loop_t *loop()
    {
        return &thread_data->loop;
    }

    bool thisIsLoop()
    {
        return std::this_thread::get_id() == thread_data->this_thread_id;
    }

    void moveToThread(std::function<void()> f)
    {
        {
            std::lock_guard<std::mutex> guard(thread_data->async_mutex);
            thread_data->async_cbk.push_back(f);
        }
        uv_async_send(&thread_data->async);
    }

private:
    static void run(void *arg)
    {
        ThreadData *thread_data = (ThreadData *)arg;
        thread_data->this_thread_id = std::this_thread::get_id();

        uv_loop_init(&thread_data->loop);

        thread_data->async.data = thread_data;
        auto onInit = [](uv_async_t *handle)
        {
            ThreadData *thread_data = (ThreadData *)handle->data;
            std::list<std::function<void()>> async_cbk;
            {
                std::lock_guard<std::mutex> guard(thread_data->async_mutex);
                async_cbk = thread_data->async_cbk;
                thread_data->async_cbk.clear();
            }
            for (auto &f : async_cbk)
            {
                f();
            }
        };
        uv_async_init(&thread_data->loop, &thread_data->async, onInit);

        thread_data->is_running = true;
        uv_run(&thread_data->loop, UV_RUN_DEFAULT);
        thread_data->is_running = false;

        uv_loop_close(&thread_data->loop);
    }
};
