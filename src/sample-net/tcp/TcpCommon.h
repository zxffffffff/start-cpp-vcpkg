#pragma once
#include "glog/logging.h"
#include <memory>
#include <list>
#include <iostream>
#include <functional>
#include <thread>
#include <mutex>

using SocketPtr = void*;

using HandleRun = std::function<void()>;
using HandleClose = std::function<void()>;
using HandleNewConn = std::function<void(SocketPtr id)>;
using HandleCloseConn = std::function<void(SocketPtr id)>;
using HandleConnOnRead = std::function<void(SocketPtr id, const char* buf, size_t len)>;
