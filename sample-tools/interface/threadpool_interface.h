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

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable : 4566)
#endif

/* 线程池抽象接口
 * 可以使用 QThread、std::thread 等第三方实现
 * 线程安全
 *
 * 警告：创建和销毁必须在同一个线程，不建议使用 shared_ptr 管理对象生命周期，请使用 unique_ptr 作为局部/成员变量。
 */
class IThreadPool
{
public:
    IThreadPool(int count) {}
    virtual ~IThreadPool() {}

    /* 动态修改线程池大小 */
    virtual void Resize(int count) = 0;

    /* 模板和虚函数不能同时使用 */
    virtual void MoveToThread(std::function<void()> f) = 0;

    /* 无法保证 future 一定执行，销毁时丢弃所有任务 */
    template <class T>
    std::future<T> MoveToThread(std::function<T()> f)
    {
        auto promise = std::make_shared<std::promise<T>>();
        auto future = promise->get_future();
        MoveToThread([=]
                     { promise->set_value(f()); });
        return future;
    }
};
