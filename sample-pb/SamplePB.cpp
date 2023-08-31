/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#include "SamplePB.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable:4566)
#endif

// 已知 macOS 问题
// - "protobuf" 和 "mysql-connector-cpp" 存在冲突，等待官方解决，暂时移除pb依赖
#ifndef __APPLE__
#include "google/protobuf/stubs/common.h"
#include "Req.pb.h"
#include "Res.pb.h"
#endif

SamplePB::SamplePB()
{
    std::cout << "你好 SamplePB." << std::endl;

#ifndef __APPLE__
    //std::cout << "protobuf version: " << google::protobuf::internal::VersionString(GOOGLE_PROTOBUF_VERSION) << std::endl;
    {
        PBReq req;
        req.set_reqid("abc");
        req.set_reqtime(123);
        auto raw = req.SerializeAsString();

        PBReq req2;
        req2.ParseFromString(raw);
        assert(req2.reqid() == req.reqid());
        assert(req2.reqtime() == req.reqtime());
    }
#endif
}
