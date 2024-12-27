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

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable:4566)
#endif

/* 线程定时器抽象接口
 * 可以使用 QTimer、std::thread 等第三方实现
 * 线程安全
 */
class IThreadTimer
{
public:
    IThreadTimer() {}
    IThreadTimer(int ms, std::function<void()> f) {}
    virtual ~IThreadTimer() {}

    virtual void SetInterval(int ms) = 0;
    virtual int  GetInterval() const = 0;

    virtual void SetCbk(std::function<void()> f) = 0;

    virtual void Start() = 0;
    virtual void Start(int ms) = 0;
    virtual void Start(int ms, std::function<void()> f) = 0;

    virtual void Stop() = 0;

    virtual bool IsRunning() const = 0;
};
