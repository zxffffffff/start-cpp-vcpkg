/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include "RedisCommon.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable:4566)
#endif

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
