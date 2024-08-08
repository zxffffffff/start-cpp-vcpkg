/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include "../interface/compression_interface.h"
#include <iomanip>
#include <sstream>
#include <cassert>

#include "snappy.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable : 4566)
#endif

class Compression_Impl : public I_Compression
{
public:
    virtual std::string Compress(const std::string &msg) override
    {
        std::string result;
        snappy::Compress(msg.c_str(), msg.size(), &result);
        return result;
    }

    virtual std::string Uncompress(const std::string &msg) override
    {
        std::string result;
        snappy::Uncompress(msg.c_str(), msg.size(), &result);
        return result;
    }
};
