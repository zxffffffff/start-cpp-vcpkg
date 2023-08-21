/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include "../interface/http_interface.h"
#include "curl/curl.h"

class IHttpClientImpl : public IHttpClient
{
public:
    virtual void InitOnce() override
    {
        curl_global_init(CURL_GLOBAL_DEFAULT);
    }

    virtual void CleanupOnce() override
    {
        curl_global_cleanup();
    }

    static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
    {
        std::stringstream &res = *((std::stringstream *)stream);
        res << std::string((const char *)ptr, (size_t)size * nmemb);
        return size * nmemb;
    }

    // 同步请求
    virtual HttpResponse Get(const std::string &url, int timeout_sec) override
    {
        std::stringstream res;

        CURL *curl = curl_easy_init();
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "User-Agent:Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.97 Safari/537.36");
        headers = curl_slist_append(headers, "Content-Type:application/x-www-form-urlencoded");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout_sec);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &res);
        CURLcode r = curl_easy_perform(curl);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        HttpResponse response;
        response.errCode = r;
        if (r != CURLE_OK)
        {
            std::stringstream ss;
            ss << __func__
               << " err=" << r
               << " msg=" << curl_easy_strerror(r)
               << " res=" << res.str();
            response.errMsg = ss.str();
        }
        response.data = res.str();
        return response;
    }

    // 同步请求
    virtual HttpResponse Post(const std::string &url, const std::string &body, int timeout_sec) override
    {
        std::stringstream res;

        CURL *curl = curl_easy_init();
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1);
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "User-Agent:Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.97 Safari/537.36");
        headers = curl_slist_append(headers, "Content-Type:application/x-www-form-urlencoded");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout_sec);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &res);
        CURLcode r = curl_easy_perform(curl);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        HttpResponse response;
        response.errCode = r;
        if (r != CURLE_OK)
        {
            std::stringstream ss;
            ss << __func__
               << " err=" << r
               << " msg=" << curl_easy_strerror(r)
               << " res=" << res.str();
            response.errMsg = ss.str();
        }
        response.data = res.str();
        return response;
    }
};
