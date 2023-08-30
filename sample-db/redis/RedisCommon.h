/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include <memory>
#include <list>
#include <map>
#include <unordered_map>
#include <iostream>
#include <functional>
#include <thread>
#include <mutex>
#include <iostream>
#include <sstream>
#include <chrono>

namespace SampleRedis
{
    using ReplyPtr = std::shared_ptr<struct Reply>;
    struct Reply
    {
        std::string str;
        std::vector<ReplyPtr> arr;
    };
}
