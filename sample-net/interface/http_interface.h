/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include "net_common.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable : 4566)
#endif

struct HttpRequest
{
    std::string method; // "GET"
    std::string path;   // "/api/search"
    std::map<std::string, std::string> parameters;
    std::string post_body;

    std::string to_string() const
    {
        std::stringstream ss;
        ss << method;
        ss << " " << path;
        for (auto ite = parameters.begin(); ite != parameters.end(); ++ite)
            ss << " " << ite->first << ":" << ite->second;
        if (post_body.size())
            ss << " " << post_body;
        return ss.str();
    }
};

struct HttpResponse
{
    int errCode = 0;
    std::string errMsg;
    std::string data;

    std::string to_string() const
    {
        std::stringstream ss;
        if (errCode)
        {
            ss << errCode << ":" << errMsg;
            if (data.size())
                ss << " data=" << data;
        }
        else
        {
            ss << data;
        }
        return ss.str();
    }
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

    virtual void SetVerbose(bool verbose) = 0;
    virtual void SetProxy(bool enable) = 0;
    virtual void SetSSLVerify(bool enable) = 0;
    virtual void SetSSLFile(const std::string &SSL_ca_file) = 0;

    virtual std::string URLEncode(const std::string &msg) const = 0;
    virtual std::string URLDecode(const std::string &msg) const = 0;

    virtual HttpResponse Get(
        const std::string &url,
        const std::vector<std::string> &headers,
        double timeout_sec) = 0;

    virtual HttpResponse Post(
        const std::string &url,
        const std::string &body,
        const std::vector<std::string> &headers,
        double timeout_sec) = 0;
};

/* HTTP (Server) Parser 抽象接口
 * 可以使用 llhttp、boost::beast::http 等第三方实现
 * 工作线程运行，非线程安全
 */
class IHttpParser
{
public:
    virtual ~IHttpParser() {}

    virtual Error ParseReq(Buffer buffer, HttpRequest &out_req) = 0; // 0, -1, 1:需要读取更多数据

    virtual std::string MakeRes(const std::string &body) = 0;
};
