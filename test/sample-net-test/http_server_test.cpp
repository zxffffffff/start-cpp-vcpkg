/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#include "gtest/gtest.h"
#include "glog/logging.h"

#include "http_server.h"
#include "impl/boost_http_parser.h"
#include "impl/libuv_tcp_server_impl.h"
#include "impl/stl_threadpool_impl.h"
using TestHttpServer = HttpServer<HttpParserImpl, ServerImpl, ThreadPoolImpl>;

#include "http_client.h"
#include "impl/curl_http_client.h"
#include "impl/stl_threadpool_impl.h"
using TestHttpClient = HttpClient<IHttpClientImpl, ThreadPoolImpl>;

TEST(HttpServerTest, GetPost)
{
    if (!google::IsGoogleLoggingInitialized())
        google::InitGoogleLogging("test");

    auto server = std::make_shared<TestHttpServer>("127.0.0.1", 12333);
    auto handler = [](ConnId, const HttpRequest& req, ServerResponseCbk cbk)
    {
        std::stringstream ss;
        ss << "res";
        ss << " method=" << req.method;
        ss << " path=" << req.path;
        for (auto ite = req.parameters.begin(); ite != req.parameters.end(); ++ite)
            ss << " " << ite->first << "=" << ite->second;
        if (req.post_body.size())
            ss << " post_body=" << req.post_body;
        cbk(ss.str());
    };
    server->SetHandleServerRequest(handler);
    server->ListenSync();

    auto response = TestHttpClient::Singleton().Get("http://127.0.0.1:12333", {}, 3).get();
    EXPECT_EQ(response.errCode, 0) << response.errMsg;
    EXPECT_EQ(response.data, "res method=GET path=/");

    auto response2 = TestHttpClient::Singleton().GetSync("http://127.0.0.1:12333/s", {{"wd", "zxffffffff"}, {"cl", "3"}}, 3);
    EXPECT_EQ(response2.errCode, 0) << response.errMsg;
    EXPECT_EQ(response2.data, "res method=GET path=/s cl=3 wd=zxffffffff");

    auto response3 = TestHttpClient::Singleton().PostSync("http://127.0.0.1:12333/test/xxx", {{"test", "1"}}, "test2=2", 3);
    EXPECT_EQ(response3.errCode, 0) << response.errMsg;
    EXPECT_EQ(response3.data, "res method=POST path=/test/xxx test=1 post_body=test2=2");

    // using namespace std::chrono_literals;
    // std::this_thread::sleep_for(30s);
}
