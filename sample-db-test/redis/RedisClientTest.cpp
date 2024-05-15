/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#include "redis/RedisClient.h"
#include "gtest/gtest.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable:4566)
#endif

namespace RedisClientTest
{
    // 跳过测试
    constexpr bool skip_test = 1;

    const char* host = "127.0.0.1";
    int port = 6379;
    // key = test-key-*
    // value = test-val-*
}
using namespace RedisClientTest;

TEST(RedisClient, ping_pong) {
    if (skip_test) return;

    RedisClient client(host, port);
    ReplyPtr p;

    ASSERT_TRUE(p = client.RunCmd("PING"));
    ASSERT_EQ(p->str, "PONG");
}

TEST(RedisClient, cmd) {
    if (skip_test) return;

    RedisClient client(host, port);
    ReplyPtr p;

    // 初始化
    ASSERT_TRUE(p = client.RunCmd("KEYS test-key-*"));
    for (auto sub : p->arr) {
        ReplyPtr p2;
        ASSERT_TRUE(p2 = client.RunCmd("DEL " + sub->str));
        EXPECT_EQ(p2->str, "1");
    }

    // 增 SET
    for (int i = 0; i < 5; ++i) {
        std::string k = "test-key-" + std::to_string(i);
        std::string v = "test-val-" + std::to_string(i);
        std::stringstream ss;
        ss << "SET " << k << " " << v;
        ASSERT_TRUE(p = client.RunCmd(ss.str()));
        EXPECT_EQ(p->str, "OK");
    }
    
    // 删 DEL
    ASSERT_TRUE(p = client.RunCmd("DEL test-key-0"));
    EXPECT_EQ(p->str, "1");
    ASSERT_TRUE(p = client.RunCmd("DEL test-key-1"));
    EXPECT_EQ(p->str, "1");

    // 改 SET、RENAME
    ASSERT_TRUE(p = client.RunCmd("RENAME test-key-2 test-key-renamed"));
    EXPECT_EQ(p->str, "OK");

    ASSERT_TRUE(p = client.RunCmd("SET test-key-3 test-val-changed"));
    EXPECT_EQ(p->str, "OK");

    // 查 GET、EXISTS、KEYS、SCAN
    // 注意：KEYS 可能导致 Redis 阻塞，建议使用 SCAN 命令渐进式的遍历所有键
    ASSERT_TRUE(p = client.RunCmd("EXISTS test-key-0"));
    EXPECT_EQ(p->str, "0");
    ASSERT_TRUE(p = client.RunCmd("EXISTS test-key-1"));
    EXPECT_EQ(p->str, "0");
    ASSERT_TRUE(p = client.RunCmd("EXISTS test-key-2"));
    EXPECT_EQ(p->str, "0");
    ASSERT_TRUE(p = client.RunCmd("EXISTS test-key-3"));
    EXPECT_EQ(p->str, "1");
    ASSERT_TRUE(p = client.RunCmd("EXISTS test-key-renamed"));
    EXPECT_EQ(p->str, "1");

    ASSERT_TRUE(p = client.RunCmd("GET test-key-1"));
    EXPECT_EQ(p->str, "");
    ASSERT_TRUE(p = client.RunCmd("GET test-key-renamed"));
    EXPECT_EQ(p->str, "test-val-2");
    ASSERT_TRUE(p = client.RunCmd("GET test-key-3"));
    EXPECT_EQ(p->str, "test-val-changed");
    ASSERT_TRUE(p = client.RunCmd("GET test-key-4"));
    EXPECT_EQ(p->str, "test-val-4");

    ASSERT_TRUE(p = client.RunCmd("KEYS test-key-*"));
    EXPECT_EQ(p->arr.size(), 5 - 2);
}

TEST(RedisClient, TTL) {
    if (skip_test) return;

    RedisClient client(host, port);
    ReplyPtr p;

    // 初始化
    ASSERT_TRUE(p = client.RunCmd("KEYS test-key-*"));
    for (auto sub : p->arr) {
        ReplyPtr p2;
        ASSERT_TRUE(p2 = client.RunCmd("DEL " + sub->str));
        EXPECT_EQ(p2->str, "1");
    }

    for (int i = 0; i < 10; ++i) {
        std::string k = "test-key-" + std::to_string(i);
        std::string v = "test-val-" + std::to_string(i);
        std::stringstream ss;
        ss << "SET " << k << " " << v;
        ASSERT_TRUE(p = client.RunCmd(ss.str()));
        EXPECT_EQ(p->str, "OK");
    }

    // 过期 EXPIRE、PERSIST、TTL
    for (int i = 0; i < 10; ++i) {
        std::string k = "test-key-" + std::to_string(i);
        std::stringstream ss;
        ss << "EXPIRE " << k << " " << std::to_string(i);
        ASSERT_TRUE(p = client.RunCmd(ss.str()));
        EXPECT_EQ(p->str, "1");
    }

    for (int i = 0; i < 10; ++i) {
        std::string k = "test-key-" + std::to_string(i);
        std::stringstream ss;
        ss << "TTL " << k;
        ASSERT_TRUE(p = client.RunCmd(ss.str()));
        EXPECT_LE(std::stoi(p->str), i);
    }
}

TEST(RedisClient, dump_restore) {
    if (skip_test) return;

    // 序列化 - 反序列化
}
