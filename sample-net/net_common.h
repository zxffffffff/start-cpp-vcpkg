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
#include <future>
#include <cassert>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include "cpp_version.h"

#if CPP_VERSION >= 2017
#include <shared_mutex>
#else /* C++11/14 */
#include <mutex>
#endif

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable:4566)
#endif

#if CPP_VERSION >= 2017
using Mutex = std::shared_mutex;
using WLock = std::unique_lock<Mutex>;
using RLock = std::shared_lock<Mutex>;
#else /* C++11/14 */
using Mutex = std::mutex;
using WLock = std::lock_guard<Mutex>;
using RLock = WLock;
#endif

/* 通用 alias */
using Error = std::shared_ptr<std::pair<int, std::string>>;
using Buffer = std::shared_ptr<std::vector<char>>;

inline Error MakeError(int state, const std::string &msg)
{
    return std::make_shared<std::pair<int, std::string>>(state, msg);
}

inline Error MakeError(bool state, const std::string &msg)
{
    return MakeError(state ? 0 : -1, msg);
}

inline Error MakeSuccess()
{
    return MakeError(0, "");
}

inline std::string ToString(Error err)
{
    if (err->first == -1)
        return err->second;
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
