/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include "../tcp_interface.h"
#include <uv.h>
#include <mutex>
#include <future>
#include <cassert>

inline Error MakeStatusError(int status)
{
    std::stringstream ss;
    ss << uv_err_name(status) << ":" << uv_strerror(status);
    return MakeError(status, ss.str());
}

class EventLoop
{
private:
    struct ThreadData
    {
        uv_loop_t loop{ 0 };
        std::promise<bool> promise_loop;

        uv_async_t async{ 0 };
        std::mutex async_mutex;
        std::vector<std::function<void()>> async_cbk;
    };
    std::unique_ptr<ThreadData> thread_data = std::make_unique<ThreadData>();

    std::atomic_bool is_start { false };
    uv_thread_t thread_id = nullptr;

public:
    /* 阻塞：等待线程运行 */
    void start()
    {
        LOG(WARNING) << __func__;

        if (is_start)
            return assert(false);
        is_start = true;

        if (thread_id)
            return assert(false);

        thread_data->promise_loop = std::promise<bool>();
        auto future = thread_data->promise_loop.get_future();
        uv_thread_create(&thread_id, run, thread_data.get());
        future.get();
    }

    /* 阻塞：等待线程结束 */
    void stop()
    {
        LOG(WARNING) << __func__;

        if (!is_start)
            return assert(false);
        is_start = false;

        if (!thread_id)
            return assert(false);

        moveToThread([this] { uv_stop(&thread_data->loop); });
        uv_thread_join(&thread_id);
        thread_id = nullptr;
    }

    uv_loop_t* loop()
    {
        return &thread_data->loop;
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
    static void run(void* arg)
    {
        LOG(WARNING) << __func__;

        ThreadData* thread_data = (ThreadData*)arg;

        uv_loop_init(&thread_data->loop);
        thread_data->promise_loop.set_value(true);

        thread_data->async.data = thread_data;
        uv_async_init(&thread_data->loop, &thread_data->async, [](uv_async_t* handle)
            {
                ThreadData* thread_data = (ThreadData*)handle->data;
        std::vector<std::function<void()>> async_cbk;
        {
            std::lock_guard<std::mutex> guard(thread_data->async_mutex);
            async_cbk = thread_data->async_cbk;
            thread_data->async_cbk.clear();
        }
        for (auto& f : async_cbk)
        {
            f();
        }
            });

        LOG(WARNING) << __func__ << " running!";
        uv_run(&thread_data->loop, UV_RUN_DEFAULT); /* running */

        LOG(WARNING) << __func__ << " closing";
        uv_loop_close(&thread_data->loop);
        LOG(WARNING) << __func__ << " closed!";
    }
};
