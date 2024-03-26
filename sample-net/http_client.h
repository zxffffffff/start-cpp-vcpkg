/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include "interface/http_interface.h"
#include "interface/threadpool_interface.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable : 4566)
#endif

namespace
{
    static std::atomic<int> g_client_cnt;
}

template <class T_IHttpClient, class T_IThreadPool>
class HttpClient
{
    std::unique_ptr<IHttpClient> client;
    std::unique_ptr<IThreadPool> threadPool;

    /* url重复的部分作为前缀 */
    std::string urlPrefix;

    std::vector<std::string> headers;
    const std::vector<std::string> default_headers = {
        "User-Agent:Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.97 Safari/537.36"
        "Content-Type:application/x-www-form-urlencoded",
    };

public:
    HttpClient(int threadPoolSize)
        : client(std::make_unique<T_IHttpClient>()), threadPool(std::make_unique<T_IThreadPool>(threadPoolSize))
    {
        if (g_client_cnt++ == 0)
            client->InitOnce();
    }

    ~HttpClient()
    {
        if (--g_client_cnt == 0)
            client->CleanupOnce();
    }

    /* 查看详细请求信息，仅用于调试 */
    virtual void SetVerbose(bool verbose) { client->SetVerbose(verbose); }

    /* 禁用 http 代理，例如 https_proxy=http://127.0.0.1:7890 */
    virtual void SetProxy(bool enable) { client->SetProxy(enable); }

    /* 禁用 SSL 验证，不建议在生产环境使用 */
    void SetSSLVerify(bool enable) { client->SetSSLVerify(enable); }

    /* Linux 不同系统路径不同，手动搜索并指定
     * # cert list copied from golang src/crypto/x509/root_unix.go
     * "/etc/ssl/certs/ca-certificates.crt",     # Debian/Ubuntu/Gentoo etc.
     * "/etc/pki/tls/certs/ca-bundle.crt",       # Fedora/RHEL
     * "/etc/ssl/ca-bundle.pem",                 # OpenSUSE
     * "/etc/openssl/certs/ca-certificates.crt", # NetBSD
     * "/etc/ssl/cert.pem",                      # OpenBSD
     * "/usr/local/share/certs/ca-root-nss.crt", # FreeBSD/DragonFly
     * "/etc/pki/tls/cacert.pem",                # OpenELEC
     * "/etc/certs/ca-certificates.crt",         # Solaris 11.2+
     */
    void SetSSLFile(const std::string &SSL_ca_file) { client->SetSSLFile(SSL_ca_file); }

    void SetPrefix(const std::string &urlPrefix) { this->urlPrefix = urlPrefix; }
    const std::string &GetPrefix() const { return urlPrefix; }

    void SetHeaders(const std::string &s) { headers.push_back(s); }
    void ClearHeaders() { headers.clear(); }

    /* 拼接参数 */
    std::string ParseParam(const std::map<std::string, std::string> &params)
    {
        std::stringstream ss;
        for (auto ite = params.begin(); ite != params.end(); ++ite)
        {
            if (ite != params.begin())
                ss << '&';
            ss << ite->first << '=' << URLEncode(ite->second);
        }
        return ss.str();
    }

    std::string URLEncode(const std::string &msg) const
    {
        return client->URLEncode(msg);
    }

    std::string URLDecode(const std::string &msg) const
    {
        return client->URLDecode(msg);
    }

    /* 异步请求 future/cbk  */
    std::future<HttpResponse> Get(
        const std::string &url,
        const std::map<std::string, std::string> &params,
        double timeout_sec = 10,
        std::function<void(HttpResponse)> cbk = nullptr)
    {
        auto onThread = [=]
        {
            auto response = GetSync(url, params, timeout_sec);
            if (cbk)
                cbk(response);
            return response;
        };
        return threadPool->MoveToThread<HttpResponse>(onThread);
    }

    std::future<HttpResponse> Post(
        const std::string &url,
        const std::map<std::string, std::string> &params,
        const std::string &body,
        double timeout_sec = 10,
        std::function<void(HttpResponse)> cbk = nullptr)
    {
        auto onThread = [=]
        {
            auto response = PostSync(url, params, body, timeout_sec);
            if (cbk)
                cbk(response);
            return response;
        };
        return threadPool->MoveToThread<HttpResponse>(onThread);
    }

    /* 同步请求 */
    HttpResponse GetSync(
        const std::string &url,
        const std::map<std::string, std::string> &params,
        double timeout_sec = 10)
    {
        std::stringstream ss;
        if (urlPrefix.size())
            ss << urlPrefix;
        ss << url;
        if (params.size())
            ss << '?' << ParseParam(params);

        return client->Get(ss.str(),
                           headers.size() ? headers : default_headers,
                           timeout_sec);
    }

    HttpResponse PostSync(
        const std::string &url,
        const std::map<std::string, std::string> &params,
        const std::string &body,
        double timeout_sec = 10)
    {
        std::stringstream ss;
        if (urlPrefix.size())
            ss << urlPrefix;
        ss << url;
        if (params.size())
            ss << '?' << ParseParam(params);

        return client->Post(ss.str(),
                            body.c_str(),
                            headers.size() ? headers : default_headers,
                            timeout_sec);
    }
};
