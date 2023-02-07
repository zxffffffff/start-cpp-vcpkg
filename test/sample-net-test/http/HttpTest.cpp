#include "http/HttpClient.h"
#include "gtest/gtest.h"

TEST(HttpClientTest, TestBaidu)
{
    auto &http = HttpClient::Singleton();

    auto s = http.Get("https://www.baidu.com", {});
    EXPECT_NE(s.find("百度"), std::string::npos);

    auto s2 = http.Get("https://www.baidu.com/s", {{"wd", "zxffffffff"}, {"cl", "3"}});
    EXPECT_NE(s2.size(), 0);

    auto s3 = http.Post("https://www.baidu.com", {}, "{}");
    EXPECT_NE(s3.size(), 0);
}
