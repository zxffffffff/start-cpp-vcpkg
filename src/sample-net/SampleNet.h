/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include "tcp_client.h"
#include "tcp_server.h"
#include "http_server.h"
#include "impl/libuv_tcp_client_impl.h"
#include "impl/libuv_tcp_server_impl.h"
#include "impl/boost_http_request.h"
#include "HttpClient.h"

class SampleNet
{
public:
    SampleNet();
};
