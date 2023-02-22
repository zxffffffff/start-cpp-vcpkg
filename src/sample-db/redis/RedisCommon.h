/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/

#pragma once
#include "Common.h"

namespace SampleRedis
{
    using ReplyPtr = std::shared_ptr<struct Reply>;
    struct Reply
    {
        std::string str;
        std::vector<ReplyPtr> arr;
    };
}
