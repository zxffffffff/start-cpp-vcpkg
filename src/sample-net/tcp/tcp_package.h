/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include <gflags/gflags.h>
#include <glog/logging.h>
#include <algorithm>
#include <vector>

/* TCP包（处理粘包、分包）
 * | head ... bodyLen | body |
 * HeadSize：固定长度包头
 * HeadFlag：用于定位包头和偏移
 * CheckBodyLen：获取Body长度
 */
class IPackage
{
public:
    virtual ~IPackage() {}

    std::vector<char>   head; /* headSize */
    std::vector<char>   body; /* bodyLen */

    virtual int         HeadSize() const = 0;
    virtual const char* HeadFlag() const = 0;     /* find(flag) */
    virtual int         HeadFlagSize() const = 0; /* find(flag) */
    virtual int         CheckBodyLen(const char* head, int len) const = 0;
};
