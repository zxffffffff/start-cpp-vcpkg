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
#include <future>
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
        std::thread::id id;

        uv_loop_t loop{0};
        std::promise<bool> promise_loop;
        std::promise<bool> promise_close;

        uv_async_t async{0};
        std::mutex async_mutex;
        std::list<std::function<void()>> async_cbk;
    };
    std::unique_ptr<ThreadData> thread_data = std::make_unique<ThreadData>();

    std::atomic_bool is_start{false};
    uv_thread_t thread_id = NULL;

public:
    /* 阻塞：等待线程运行 */
    void start()
    {
        // LOG(WARNING) << __func__;

        if (is_start)
            return assert(false); /* 必须按顺序调用 */
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
        // LOG(WARNING) << __func__;

        if (!is_start)
            return assert(false); /* 必须按顺序调用 */
        is_start = false;

        if (!thread_id)
            return assert(false);

        thread_data->promise_close = std::promise<bool>();
        auto future = thread_data->promise_close.get_future();
        moveToThread([this]
                     { uv_stop(&thread_data->loop); });
        /* 存在线程同步问题 */
        /* win单例对象析构时线程可能被杀，使用future判断 */
        auto status = future.wait_for(std::chrono::milliseconds(100));
        if (status != std::future_status::ready)
            uv_thread_join(&thread_id);
        thread_id = NULL;
    }

    uv_loop_t *loop()
    {
        return &thread_data->loop;
    }

    bool thisIsLoop()
    {
        return std::this_thread::get_id() == thread_data->id;
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
        // LOG(WARNING) << __func__;

        ThreadData *thread_data = (ThreadData *)arg;
        thread_data->id = std::this_thread::get_id();

        uv_loop_init(&thread_data->loop);
        thread_data->promise_loop.set_value(true);

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

        // LOG(WARNING) << __func__ << " running!";
        uv_run(&thread_data->loop, UV_RUN_DEFAULT); /* running */

        // LOG(WARNING) << __func__ << " closing";
        uv_loop_close(&thread_data->loop);
        // LOG(WARNING) << __func__ << " closed!";

        thread_data->promise_close.set_value(true);
    }
};
