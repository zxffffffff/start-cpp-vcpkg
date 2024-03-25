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
 */
class IThreadPool
{
public:
    IThreadPool(int count) {}
    virtual ~IThreadPool() {}

    /* future<bool> 表示 f 是否执行 */
    virtual std::future<bool> MoveToThread(std::function<void()> f) = 0;

    /* 模板和虚函数不能同时使用 */
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
