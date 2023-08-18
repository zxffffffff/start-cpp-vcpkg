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
