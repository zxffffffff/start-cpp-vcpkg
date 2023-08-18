/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include "curl/curl.h"
#include <memory>
#include <list>
#include <map>
#include <unordered_map>
#include <iostream>
#include <functional>
#include <thread>
#include <mutex>
#include <iostream>
#include <sstream>
#include <chrono>

class HttpClientPrivate
{
public:
    static void global_init()
    {
        curl_global_init(CURL_GLOBAL_DEFAULT);
    }

    static void global_cleanup()
    {
        curl_global_cleanup();
    }

    static size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream)
    {
        std::stringstream& res = *((std::stringstream*)stream);
        res << std::string((const char*)ptr, (size_t)size * nmemb);
        return size * nmemb;
    }

    // 同步请求
    static std::string Get(const std::string& url, int timeout_sec, std::string& error)
    {
        std::stringstream res;

        CURL* curl = curl_easy_init();
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "User-Agent:Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.97 Safari/537.36");
        headers = curl_slist_append(headers, "Content-Type:application/x-www-form-urlencoded");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout_sec);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &res);
        CURLcode r = curl_easy_perform(curl);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        if (r != CURLE_OK) {
            std::stringstream ss;
            ss << __func__
                << " err=" << r
                << " msg=" << curl_easy_strerror(r)
                << " res=" << res.str();
            error = ss.str();
            return "";
        }
        return res.str();
    }

    // 同步请求
    static std::string Post(const std::string& url, const char* body, int timeout_sec, std::string& error)
    {
        std::stringstream res;

        CURL* curl = curl_easy_init();
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1);
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "User-Agent:Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.97 Safari/537.36");
        headers = curl_slist_append(headers, "Content-Type:application/x-www-form-urlencoded");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout_sec);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &res);
        CURLcode r = curl_easy_perform(curl);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        if (r != CURLE_OK) {
            std::stringstream ss;
            ss << __func__
                << " err=" << r
                << " msg=" << curl_easy_strerror(r)
                << " res=" << res.str();
            error = ss.str();
            return "";
        }
        return res.str();
    }
};
