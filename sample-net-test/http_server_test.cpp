/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#include "gtest/gtest.h"

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
    TestHttpClient http(4);
    // http.SetVerbose(true);
    http.SetProxy(false);

    auto server = std::make_shared<TestHttpServer>("127.0.0.1", 12333);
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
    server->SetHandleServerRequest(handler);
    bool ok = server->ListenSync();
    ASSERT_TRUE(ok);

    auto response = http.Get("http://127.0.0.1:12333", {}, 3).get();
    EXPECT_EQ(response.errCode, 0) << response.errMsg;
    EXPECT_EQ(response.data, "res method=GET path=/");

    auto response2 = http.GetSync("http://127.0.0.1:12333/s", {{"wd", "zxffffffff"}, {"cl", "3"}}, 3);
    EXPECT_EQ(response2.errCode, 0) << response.errMsg;
    EXPECT_EQ(response2.data, "res method=GET path=/s cl=3 wd=zxffffffff");

    auto response3 = http.PostSync("http://127.0.0.1:12333/test/xxx", {{"test", "1"}}, "test2=2", 3);
    EXPECT_EQ(response3.errCode, 0) << response.errMsg;
    EXPECT_EQ(response3.data, "res method=POST path=/test/xxx test=1 post_body=test2=2");

    // using namespace std::chrono_literals;
    // std::this_thread::sleep_for(30s);
}
