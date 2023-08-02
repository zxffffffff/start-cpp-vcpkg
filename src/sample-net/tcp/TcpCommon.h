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
#include <unordered_map>
#include <iostream>
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>
#include <future>
#include <cassert>

using SocketPtr = void*;

using HandleRun = std::function<void(bool run_success)>;
using HandleClose = std::function<void()>;
using HandleNewConn = std::function<void(SocketPtr id)>;
using HandleCloseConn = std::function<void(SocketPtr id)>;
using HandleConnOnRead = std::function<void(SocketPtr id, const char* buf, size_t len)>;
using HandleConnOnWrite = std::function<void(SocketPtr id, int status)>;
