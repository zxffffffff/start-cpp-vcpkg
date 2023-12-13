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

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable:4566)
#endif

class IHttpClientImpl : public IHttpClient
{
    long enableSSLVerify = 1;

public:
    virtual void InitOnce() override
    {
        curl_global_init(CURL_GLOBAL_DEFAULT);
    }

    virtual void CleanupOnce() override
    {
        curl_global_cleanup();
    }

    virtual void SetSSLVerify(bool enable) override
    {
        enableSSLVerify = enable ? 1 : 0;
    }

    virtual std::string URLEncode(const std::string &msg) const override
    {
        std::string ret;
        CURL *curl = curl_easy_init();
        if (curl)
        {
            char *output = curl_easy_escape(curl, msg.c_str(), msg.size());
            if (output)
            {
                ret = output;
                curl_free(output);
            }
            curl_easy_cleanup(curl);
        }
        return ret;
    }

    virtual std::string URLDecode(const std::string &msg) const override
    {
        std::string ret;
        CURL *curl = curl_easy_init();
        if (curl)
        {
            int outlength;
            char *output = curl_easy_unescape(curl, msg.c_str(), msg.size(), &outlength);
            if (output)
            {
                ret = std::string(output, outlength);
                curl_free(output);
            }
            curl_easy_cleanup(curl);
        }
        return ret;
    }

    static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
    {
        std::stringstream &res = *((std::stringstream *)stream);
        res << std::string((const char *)ptr, (size_t)size * nmemb);
        return size * nmemb;
    }

    // 同步请求
    virtual HttpResponse Get(
        const std::string &url,
        const std::vector<std::string> &_headers,
        double timeout_sec) override
    {
        std::stringstream res;

        CURL *curl = curl_easy_init();
        // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, enableSSLVerify);
        struct curl_slist *headers = NULL;
        for (auto &s : _headers)
            headers = curl_slist_append(headers, s.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, int(timeout_sec * 750));
        curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, int(timeout_sec * 1000));
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
    virtual HttpResponse Post(
        const std::string &url,
        const std::string &body,
        const std::vector<std::string> &_headers,
        double timeout_sec) override
    {
        std::stringstream res;

        CURL *curl = curl_easy_init();
        // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1);
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, enableSSLVerify);
        struct curl_slist *headers = NULL;
        for (auto &s : _headers)
            headers = curl_slist_append(headers, s.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, int(timeout_sec * 750));
        curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, int(timeout_sec * 1000));
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
