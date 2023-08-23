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
#include <memory>
#include <shared_mutex>
#include <future>
#include <cassert>
#include <sstream>
#include <vector>
#include <map>
#include <set>

/* 通用 alias */
using Error = std::shared_ptr<std::pair<int, std::string>>;
using Buffer = std::shared_ptr<std::vector<char>>;

inline Error MakeError(int state, const std::string &msg)
{
    return std::make_shared<std::pair<int, std::string>>(state, msg);
}

inline Error MakeError(bool state, const std::string &msg)
{
    return MakeError(-1, msg);
}

inline Error MakeSuccess()
{
    return MakeError(0, "");
}

inline std::string ToString(Error err)
{
    if (err->first)
        return std::to_string(err->first) + ":" + err->second;
    return "";
}

inline Buffer MakeBuffer()
{
    return std::make_shared<std::vector<char>>();
}

inline Buffer MakeBuffer(const char *buf, int len)
{
    Buffer buffer = std::make_shared<std::vector<char>>(len);
    if (len > 0)
        memcpy(buffer->data(), buf, len);
    return buffer;
}

inline Buffer MakeBuffer(const std::string &s)
{
    return MakeBuffer(s.c_str(), s.size());
}

template <class T>
inline std::future<T> MakeFuture(const T &value)
{
    std::promise<T> promise;
    std::future<T> future = promise.get_future();
    promise.set_value(value);
    return future;
}

using ConnId = size_t; /* 可以用内存地址 */
