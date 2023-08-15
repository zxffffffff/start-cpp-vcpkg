/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/

#include "HttpClient.h"
#include "HttpClient_p.h"

std::once_flag flag;

HttpClient::HttpClient()
{
    HttpClientPrivate::global_init();
}

HttpClient::~HttpClient()
{
    HttpClientPrivate::global_cleanup();
}

std::string HttpClient::Get(
    const std::string& url,
    const std::map<std::string, std::string>& params,
    int timeout /*= 10*/
)
{
    std::string complete_url = url + "?" + ParseParam(params);
    return HttpClientPrivate::Get(complete_url.c_str(), timeout, lastError);
}

std::string HttpClient::Post(
    const std::string& url,
    const std::map<std::string, std::string>& params,
    const std::string& body,
    int timeout /*= 10*/
)
{
    std::string complete_url = url + "?" + ParseParam(params);
    return HttpClientPrivate::Post(complete_url.c_str(), body.c_str(), timeout, lastError);
}

std::string HttpClient::ParseParam(const std::map<std::string, std::string>& params)
{
    if (params.empty())
        return "";
    std::stringstream ss;
    for (auto ite = params.begin(); ite != params.end(); ++ite) {
        if (ite != params.begin())
            ss << "&";
        ss << ite->first << "=" << ite->second;
    }
    return ss.str();
}
