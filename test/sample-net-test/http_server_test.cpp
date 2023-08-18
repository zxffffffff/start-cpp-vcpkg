/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#include "gtest/gtest.h"

#include "impl/boost_http_request.h"
#include "impl/libuv_tcp_server_impl.h"
#include "impl/stl_threadpool_impl.h"
#include "http_server.h"

#include "HttpClient.h"

using namespace std::chrono_literals;

using TestHttpServer = HttpServer<HttpParserImpl, ServerImpl, ThreadPoolImpl>;

TEST(HttpServerTest, GetPost)
{
    if (!google::IsGoogleLoggingInitialized())
        google::InitGoogleLogging("test");

    auto server = std::make_shared<TestHttpServer>("127.0.0.1", 12345);
    server->ListenSync();

    auto s = HttpClient::Singleton().Get("https://127.0.0.1:12345", {});
    EXPECT_NE(s.size(), 0);

    auto s2 = HttpClient::Singleton().Get("https://127.0.0.1:12345/s", { {"wd", "zxffffffff"}, {"cl", "3"} });
    EXPECT_NE(s2.size(), 0);

    auto s3 = HttpClient::Singleton().Post("https://127.0.0.1:12345", {}, "{}");
    EXPECT_NE(s3.size(), 0);
}
