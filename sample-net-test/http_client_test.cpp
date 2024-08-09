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

TEST(HttpClientTest, curl_version)
{
    std::string ver = curl_version();
    ASSERT_FALSE(ver.empty());

    std::cout << "curl version: " << ver << std::endl;
}

TEST(HttpClientTest, TestBaidu)
{
    TestHttpClient http(4);
    TestHttpClient http2(4);
    http.SetProxy(false);
    http2.SetProxy(false);

    /* 低端设备可能单测失败 */
    if (Hardware::GetCPUs() < 8)
    {
        http.SetVerbose(true);
        http2.SetVerbose(true);
    }

    auto response = http.Get("https://www.baidu.com", {}).get();
    EXPECT_EQ(response.errCode, 0) << response.errMsg;
    EXPECT_NE(response.data.find("百度"), std::string::npos);

    auto response2 = http.GetSync("https://www.baidu.com/s", {{"wd", "zxffffffff"}, {"cl", "3"}});
    EXPECT_EQ(response2.errCode, 0) << response.errMsg;
    EXPECT_NE(response2.data.size(), 0);

    auto response3 = http2.PostSync("https://www.baidu.com", {}, "{}");
    EXPECT_EQ(response3.errCode, 0) << response.errMsg;
    EXPECT_NE(response3.data.size(), 0);
}

TEST(HttpClientTest, URLEncode)
{
    TestHttpClient http(4);

    /* 低端设备可能单测失败 */
    if (Hardware::GetCPUs() < 8)
        http.SetVerbose(true);

    std::string raw = "abc123!@#$%^&*()_-+=*/\\'\"?你好";

    std::string encode = http.URLEncode(raw);
    EXPECT_EQ(encode, "abc123%21%40%23%24%25%5E%26%2A%28%29_-%2B%3D%2A%2F%5C%27%22%3F%E4%BD%A0%E5%A5%BD");

    std::string decode = http.URLDecode(encode);
    EXPECT_EQ(decode, raw);
}
