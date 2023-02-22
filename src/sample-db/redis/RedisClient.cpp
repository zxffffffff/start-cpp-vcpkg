/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/

#include "RedisClient.h"
#include "RedisClient_p.h"

RedisClient::RedisClient(const char* host, int port)
    : pri(std::make_unique<RedisClientPrivate>(host, port))
{
}

RedisClient::~RedisClient()
{
}

ReplyPtr RedisClient::RunCmd(const std::string& cmd)
{
    return pri->RunCmd(cmd);
}
