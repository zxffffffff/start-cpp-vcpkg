#pragma once
#include "HttpCommon.h"
#include "curl/curl.h"

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
        std::stringstream& ss = *((std::stringstream*)stream);
        ss << std::string((const char*)ptr, (size_t)size * nmemb);
        return size * nmemb;
    }

    // 同步请求
    static std::string Get(const char* url, int timeout)
    {
        std::stringstream res;

        CURL* curl = curl_easy_init();
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &res);
        CURLcode r = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (r != CURLE_OK) {
            LOG(ERROR) << curl_easy_strerror(r);
            return "";
        }
        return res.str();
    }

    // 同步请求
    static std::string Post(const char* url, const char* body, int timeout)
    {
        std::stringstream res;

        CURL* curl = curl_easy_init();
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_POST, 1);
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "User-Agent:Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.97 Safari/537.36");
        headers = curl_slist_append(headers, "Content-Type:application/x-www-form-urlencoded");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &res);
        CURLcode r = curl_easy_perform(curl);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        if (r != CURLE_OK) {
            LOG(ERROR) << curl_easy_strerror(r);
            return "";
        }
        return res.str();
    }
};
