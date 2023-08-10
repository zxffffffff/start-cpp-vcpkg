/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include <uv.h>
#include <mutex>
#include <future>

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
    void start();

    /* 阻塞：等待线程结束 */
    void stop();

    uv_loop_t* loop()
    {
        return &thread_data->loop;
    }

    void moveToThread(std::function<void()> f);

private:
    static void run(void* arg);
};
