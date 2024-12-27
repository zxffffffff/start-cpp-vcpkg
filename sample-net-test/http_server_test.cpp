/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#include "gtest/gtest.h"

#include "common.h"
#include "platform.h"
#include "hardware.h"
#include "log.h"

#include "http_server.h"
#include "impl/boost_http_parser.h"
#include "impl/libuv_tcp_server_impl.h"
#include "impl/stl_threadpool_impl.h"
using TestHttpServer = HttpServer<HttpParserImpl, ServerImpl>;

#include "http_client.h"
#include "impl/curl_http_client.h"
#include "impl/stl_threadpool_impl.h"
using TestHttpClient = HttpClient<HttpClientImpl, ThreadPoolImpl>;

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable : 4566)
#endif

/* 警告：Google Test 仅在 *nix 上线程安全，Windows 或其他平台不支持多线程断言 */
TEST(HttpServerTest, GetPost)
{
    Log::InitCrashReport();

    for (int i = 0; i < 100; ++i)
    {
        /* 随机端口，减少端口被占用概率 */
        std::string str_ip = "127.0.0.1";
        int n_port = Common::Random(10000, 49151);

        TestHttpClient http(4);
        http.SetProxy(false);

        /* 低端设备更容易复现问题 */
        if (Hardware::GetCPUs() < 8)
            http.SetVerbose(true);

        TestHttpServer server(str_ip, n_port);
        auto handler = [](ConnId, Error err, std::shared_ptr<HttpRequest> req, ServerResponseCbk cbk)
        {
            EXPECT_EQ(err->first, 0);
            std::stringstream ss;
            ss << "res";
            ss << " method=" << req->method;
            ss << " path=" << req->path;
            for (auto ite = req->parameters.begin(); ite != req->parameters.end(); ++ite)
                ss << " " << ite->first << "=" << ite->second;
            if (req->post_body.size())
                ss << " post_body=" << req->post_body;
            cbk(ss.str());
        };
        server.SetHandleServerRequest(handler);
        bool ok = server.ListenSync();
        if (!ok)
        {
            /* 可能端口被占用 */
            std::cerr << "server listen failed, port=" << n_port << std::endl;
            continue;
        }

        std::string str_http = "http://" + str_ip + ":" + std::to_string(n_port);

        auto response = http.Get(str_http, {}).get();
        EXPECT_EQ(response.errCode, 0) << response.errMsg;
        EXPECT_EQ(response.data, "res method=GET path=/");

        auto response2 = http.GetSync(str_http + "/s", {{"wd", "zxffffffff"}, {"cl", "3"}});
        EXPECT_EQ(response2.errCode, 0) << response.errMsg;
        EXPECT_EQ(response2.data, "res method=GET path=/s cl=3 wd=zxffffffff");

        auto response3 = http.PostSync(str_http + "/test/xxx", {{"test", "1"}}, "test2=2");
        EXPECT_EQ(response3.errCode, 0) << response.errMsg;
        EXPECT_EQ(response3.data, "res method=POST path=/test/xxx test=1 post_body=test2=2");

        ok = server.CloseSync();
        ASSERT_TRUE(ok);
    }
}
