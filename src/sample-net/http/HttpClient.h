#pragma once
#include "HttpCommon.h"

class HttpClient
{
private:
    HttpClient();
    ~HttpClient();

public:
    static HttpClient& Singleton()
    {
        static HttpClient ins;
        return ins;
    }

    // 同步请求
    std::string Get(
        const std::string& url,
        const std::map<std::string, std::string>& params,
        int timeout = 10
    );

    // 同步请求
    std::string Post(
        const std::string& url,
        const std::map<std::string, std::string>& params,
        const std::string& body,
        int timeout = 10
    );

    static std::string ParseParam(const std::map<std::string, std::string>& params);
};
