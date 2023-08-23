/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#include "gtest/gtest.h"
#include "glog/logging.h"

#include "http_client.h"
#include "impl/curl_http_client.h"
#include "impl/boost_threadpool_impl.h"
using TestHttpClient = HttpClient<IHttpClientImpl, ThreadPoolImpl<8>>;

TEST(HttpClientTest, TestBaidu)
{
    if (!google::IsGoogleLoggingInitialized())
        google::InitGoogleLogging("test");

    auto response = TestHttpClient::Singleton().Get("https://www.baidu.com", {}).get();
    EXPECT_EQ(response.errCode, 0) << response.errMsg;
    EXPECT_NE(response.data.find("百度"), std::string::npos);

    auto response2 = TestHttpClient::Singleton().GetSync("https://www.baidu.com/s", {{"wd", "zxffffffff"}, {"cl", "3"}});
    EXPECT_EQ(response2.errCode, 0) << response.errMsg;
    EXPECT_NE(response2.data.size(), 0);

    auto response3 = TestHttpClient::Singleton().PostSync("https://www.baidu.com", {}, "{}");
    EXPECT_EQ(response3.errCode, 0) << response.errMsg;
    EXPECT_NE(response3.data.size(), 0);
}
