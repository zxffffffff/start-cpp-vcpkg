#pragma once
#include "glog/logging.h"
#include <memory>
#include <list>
#include <iostream>
#include <functional>
#include <format>

using SocketPtr = size_t;

using HandleRun = std::function<void(SocketPtr id)>;
using HandleClose = std::function<void(SocketPtr id)>;
using HandleNewConn = std::function<void(SocketPtr id)>;
using HandleCloseConn = std::function<void(SocketPtr id)>;
using HandleConnOnRead = std::function<void(SocketPtr id, const char* buf, size_t len)>;
using HandleConnOnWrite = std::function<void(SocketPtr id, const char* buf, size_t len)>;

struct WriteReq
{
    uv_write_t req;
    uv_buf_t buf;
    HandleConnOnWrite handleConnOnWrite;
};
