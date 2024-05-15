/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include "glog/logging.h"
#include "RedisCommon.h"
#include "hiredis/hiredis.h"
#include "Chrono.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable:4566)
#endif

using namespace SampleRedis;

/* 支持同步和异步回调的方式，为了方便这里使用同步函数
 * 核心3步：redisConnect -> redisCommand -> freeReplyObject
 * 支持 libuv、glib、qt 等多种 C 库共同使用
 */
class RedisClientPrivate
{
private:
    /* Unix 提供了额外的接口
     * redisConnect / redisConnectUnix
     */
    redisContext* ctx = NULL;

public:
    RedisClientPrivate(const char* host, int port)
    {
        ctx = redisConnect(host, port);
        if (ctx == NULL || ctx->err) {
            if (ctx) {
                LOG(ERROR) << "[redisConnect] " << ctx->errstr;
                redisFree(ctx);
                ctx = NULL;
            }
            else {
                LOG(ERROR) << "[redisConnect] Can't allocate redis context";
            }
        }
    }

    ~RedisClientPrivate()
    {
        if (ctx) {
            redisFree(ctx);
        }
    }

    ReplyPtr RunCmd(const std::string& cmd)
    {
        LOG(INFO) << "[RunCmd cmd] " << cmd;

        if (ctx == NULL || ctx->err) {
            LOG(ERROR) << "[RunCmd] ctx err";
            return nullptr;
        }

        Chrono chrono;
        redisReply* reply = (redisReply*)redisCommand(ctx, cmd.c_str());
        auto use_time = chrono.stop();
        if (reply == NULL || ctx->err) {
            LOG(ERROR) << "[RunCmd] " << ctx->errstr;
            return nullptr;
        }

        auto p = ParseReply(reply);
        LOG(INFO) << "[RunCmd ret] use_time=" << use_time << "ms type=" << reply->type << (p->str.empty() ? "" : " str=" + p->str);
        freeReplyObject(reply);
        return p;
    }

private:
    ReplyPtr ParseReply(redisReply* reply)
    {
        ReplyPtr p = std::make_shared<Reply>();
        switch (reply->type)
        {
        case REDIS_REPLY_INTEGER:
            p->str = std::to_string(reply->integer);
            break;
        case REDIS_REPLY_ERROR:
        case REDIS_REPLY_STRING:
        case REDIS_REPLY_VERB:      // 支持 char[4] 和 string 
        case REDIS_REPLY_DOUBLE:    // 支持 double 和 string
        case REDIS_REPLY_STATUS:
            p->str = std::string(reply->str);
            break;
        case REDIS_REPLY_ARRAY:
            p->arr.resize(reply->elements);
            for (size_t i = 0; i < reply->elements; ++i) {
                redisReply* ele = reply->element[i];
                p->arr[i] = ParseReply(ele);
            }
            break;
        case REDIS_REPLY_NIL:
            break;
        default:
            if (reply->str)
                p->str = std::string(reply->str);
            LOG(WARNING) << "[ParseReply warning] type=" << reply->type << " str=" << p->str;
            break;
        }
        return p;
    }
};
