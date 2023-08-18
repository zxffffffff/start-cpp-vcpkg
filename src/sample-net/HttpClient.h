/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/

#pragma once
#include "Common.h"

class HttpClient
{
private:
    HttpClient();
    ~HttpClient();

    std::string lastError;

public:
    static HttpClient& Singleton()
    {
        static HttpClient ins;
        return ins;
    }

    /* 同步请求 */
    std::string Get(
        const std::string& url,
        const std::map<std::string, std::string>& params,
        int timeout = 10
    );

    /* 同步请求 */
    std::string Post(
        const std::string& url,
        const std::map<std::string, std::string>& params,
        const std::string& body,
        int timeout = 10
    );

    /* 获取错误信息 */
    std::string LastError() const { return lastError; }

    static std::string ParseParam(const std::map<std::string, std::string>& params);
};
