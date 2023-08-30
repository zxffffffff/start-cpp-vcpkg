/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include <algorithm>
#include <string>
#include <functional>
#include <future>

/* 线程池抽象接口
 * 可以使用 QThread、std::thread 等第三方实现
 * 线程安全
 */

class IThreadPool
{
public:
    IThreadPool() { /* 开启 */ }
    virtual ~IThreadPool() { /* 结束 */ }

    virtual std::future<void> MoveToThread(std::function<void()> f) = 0;

    /* 模板和虚函数不能同时使用 */
    template<class T>
    std::future<T> MoveToThread(std::function<T()> f)
    {
        auto promise = std::make_shared<std::promise<T>>();
        auto future = promise->get_future();
        MoveToThread([=] { promise->set_value(f()); });
        return future;
    }
};
