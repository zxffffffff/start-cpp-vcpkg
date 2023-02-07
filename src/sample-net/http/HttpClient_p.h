#pragma once
#include "HttpCommon.h"
#include "curl/curl.h"

class HttpClientPrivate
{
private:

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
            LOG(ERROR) << "curl_easy_perform error " << curl_easy_strerror(r);
            return "";
        }
        return res.str();
    }

    static std::string Post(const char* url, const char* json, int timeout)
    {
        std::stringstream res;

        CURL* curl = curl_easy_init();
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_POST, 1);
        auto headers = curl_slist_append(NULL, "content-type:application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &res);
        CURLcode r = curl_easy_perform(curl);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        if (r != CURLE_OK) {
            LOG(ERROR) << "curl_easy_perform error " << curl_easy_strerror(r);
            return "";
        }
        return res.str();
    }
};
