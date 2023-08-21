/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include "net_common.h"

struct HttpRequest
{
    std::string method; // "GET"
    std::string path;   // "/api/search"
    std::map<std::string, std::string> parameters;
    std::string post_body;
};

struct HttpResponse
{
    int errCode = 0;
    std::string errMsg;
    std::string data;
};

/* HTTP 抽象接口
 * 可以使用 curl、boost::beast::http 等第三方实现
 * 工作线程运行，非线程安全
 */
class IHttpClient
{
public:
    virtual ~IHttpClient() {}

    virtual void InitOnce() {}
    virtual void CleanupOnce() {}

    virtual HttpResponse Get(const std::string &url, int timeout_sec) = 0;
    virtual HttpResponse Post(const std::string &url, const std::string &body, int timeout_sec) = 0;
};

/* HTTP (Server) Parser 抽象接口
 * 可以使用 llhttp、boost::beast::http 等第三方实现
 * 工作线程运行，非线程安全
 */
class IHttpParser
{
public:
    virtual ~IHttpParser() {}

    virtual Error ParseReq(Buffer buffer, HttpRequest& out_req) = 0;
    virtual std::string MakeRes(std::string body) = 0;
};
