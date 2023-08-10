/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include "../tcp_interface.h"
#include "libuv_event_loop.h"

class ClientImpl : public IClient
{
public:
    /* 线程事件循环 */
    EventLoop eventLoop;

    /* socket */
    uv_tcp_t socket{ 0 };
    sockaddr_in dest{ 0 };
    uv_connect_t connection{ 0 };

    /* 线程通信 */
    std::shared_ptr<std::promise<Error>> connect_promise;
    std::shared_ptr<std::promise<Error>> close_promise;

    struct WriteTask
    {
        std::shared_ptr<std::promise<Error>> promise;
        Buffer write_buf;
    };
    using SpWriteTask = std::shared_ptr<std::vector<WriteTask>>;
    SpWriteTask write_tasks;
    std::mutex write_task_mutex;

public:
    ClientImpl();
    virtual ~ClientImpl();

    virtual std::future<Error> Connect(const std::string& ip, int port) override;
    virtual std::future<Error> Close() override;
    virtual std::future<Error> Write(Buffer buffer) override;
};
