/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include "RedisCommon.h"

using namespace SampleRedis;

class RedisClientPrivate;
class RedisClient
{
public:
    /* [阻塞] 创建一个 Redis 连接
     * host = "127.0.0.1"
     * port = 6379
     */
    RedisClient(const char* host, int port);
    ~RedisClient();

    ReplyPtr RunCmd(const std::string& cmd);

private:
    std::unique_ptr<RedisClientPrivate> pri;
};
