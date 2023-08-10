/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include <gflags/gflags.h>
#include <glog/logging.h>
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

    virtual std::future<bool> MoveToThread(std::function<bool()> task) = 0;
};
