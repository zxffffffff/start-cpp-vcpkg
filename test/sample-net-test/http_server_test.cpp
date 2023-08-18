/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#include "gtest/gtest.h"
#include "glog/logging.h"

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

    auto server = std::make_shared<TestHttpServer>("127.0.0.1", 12333);
    auto handler = [](ConnId, const HttpRequest& req)
    {
        std::stringstream ss;
        ss << "res";
        ss << " method=" << req.method;
        ss << " path=" << req.path;
        for (auto ite = req.parameters.begin(); ite != req.parameters.end(); ++ite)
            ss << " " << ite->first << "=" << ite->second;
        if (req.post_body.size())
            ss << " post_body=" << req.post_body;
        return ss.str();
    };
    server->SetHandleServerRequest(handler);
    server->ListenSync();

    auto s = HttpClient::Singleton().Get("http://127.0.0.1:12333", {}, 3);
    if (s.empty())
        std::cerr << HttpClient::Singleton().LastError() << std::endl;
    EXPECT_EQ(s, "res method=GET path=/");

    auto s2 = HttpClient::Singleton().Get("http://127.0.0.1:12333/s", { {"wd", "zxffffffff"}, {"cl", "3"} }, 3);
    if (s2.empty())
        std::cerr << HttpClient::Singleton().LastError() << std::endl;
    EXPECT_EQ(s2, "res method=GET path=/s cl=3 wd=zxffffffff");

    auto s3 = HttpClient::Singleton().Post("http://127.0.0.1:12333/test/xxx", {{"test", "1"}}, "test2=2", 3);
    if (s3.empty())
        std::cerr << HttpClient::Singleton().LastError() << std::endl;
    EXPECT_EQ(s3, "res method=POST path=/test/xxx test=1 test2=2 post_body=test2=2");

    //std::this_thread::sleep_for(30s);
}
