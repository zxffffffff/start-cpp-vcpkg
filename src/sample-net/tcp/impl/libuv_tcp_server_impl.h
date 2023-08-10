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

class ServerImpl : public IServer
{
public:
    /* 线程事件循环 */
    EventLoop eventLoop;

    /* socket */
    uv_tcp_t socket{ 0 };
    sockaddr_in addr{ 0 };
    const int listen_backlog = 128;
    std::vector<ConnId> connections; /* uv_tcp_t* */

    /* 线程通信 */
    std::shared_ptr<std::promise<Error>> listen_promise;
    std::shared_ptr<std::promise<Error>> close_promise;

    struct WriteTask
    {
        std::shared_ptr<std::promise<Error>> promise;
        ConnId connId;
        Buffer write_buf;
    };
    using SpWriteTask = std::shared_ptr<std::vector<WriteTask>>;
    SpWriteTask write_tasks;
    std::mutex write_task_mutex;

public:
    ServerImpl();
    virtual ~ServerImpl();

    virtual std::future<Error> Listen(const std::string& ip, int port) override;
    virtual std::future<Error> Close() override;
    virtual std::future<Error> Write(ConnId connId, Buffer buffer) override;
};
