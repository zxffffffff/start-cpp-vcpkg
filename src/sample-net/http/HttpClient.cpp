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

HttpClient& HttpClient::Singleton()
{
    static HttpClient ins;
    return ins;
}

std::string HttpClient::Get(
    const std::string& url,
    const std::map<std::string, std::string>& params,
    int timeout /*= 10*/
)
{
    std::string complete_url = url + ParseParam(params);
    return HttpClientPrivate::Get(complete_url.c_str(), timeout);
}

std::string HttpClient::Post(
    const std::string& url,
    const std::map<std::string, std::string>& params,
    const std::string& json,
    int timeout /*= 10*/
)
{
    std::string complete_url = url + ParseParam(params);
    return HttpClientPrivate::Post(complete_url.c_str(), json.c_str(), timeout);
}

std::string HttpClient::ParseParam(const std::map<std::string, std::string>& params)
{
    if (params.empty())
        return "";
    std::stringstream ss;
    for (auto ite = params.begin(); ite != params.end(); ++ite) {
        if (ite == params.begin())
            ss << "?";
        else
            ss << "&";
        ss << ite->first << "=" << ite->second;
    }
    return ss.str();
}
