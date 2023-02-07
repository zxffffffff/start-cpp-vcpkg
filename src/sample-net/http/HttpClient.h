#pragma once
#include "HttpCommon.h"

class HttpClient
{
private:
    HttpClient();
    ~HttpClient();

public:
    static HttpClient& Singleton();

    /* ×èÈûÇëÇó
     * url : "https://www.baidu.com"
     * json : {}
     * timeout : sec
     */

    std::string Get(
        const std::string& url,
        const std::map<std::string, std::string>& params,
        int timeout = 10
    );

    std::string Post(
        const std::string& url,
        const std::map<std::string, std::string>& params,
        const std::string& json,
        int timeout = 10
    );

private:
    std::string ParseParam(const std::map<std::string, std::string>& params);
};
