/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#include "RedisClient.h"
#include "RedisClient_p.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable:4566)
#endif

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
