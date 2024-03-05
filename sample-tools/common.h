/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/hex.hpp>

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable : 4566)
#endif

class Common
{
public:
    /* 随机guid (00000000-0000-0000-0000-000000000000) */
    static std::string GenGuid()
    {
        boost::uuids::random_generator gen;
        return boost::uuids::to_string(gen());
    }
    
    static std::string stringToHex(const std::vector<char> &input)
    {
        std::string out;
        boost::algorithm::hex(input.begin(), input.end(), std::back_inserter(out));
        return out;
    }

    static std::string stringToHex(const std::string &input)
    {
        std::string out;
        boost::algorithm::hex(input.begin(), input.end(), std::back_inserter(out));
        return out;
    }

    static std::string stringToHex(const char *input)
    {
        std::string out;
        boost::algorithm::hex(input, std::back_inserter(out));
        return out;
    }

    static std::string hexToString(const std::vector<char> &input)
    {
        std::string out;
        boost::algorithm::unhex(input.begin(), input.end(), std::back_inserter(out));
        return out;
    }

    static std::string hexToString(const std::string &input)
    {
        std::string out;
        boost::algorithm::unhex(input.begin(), input.end(), std::back_inserter(out));
        return out;
    }

    static std::string hexToString(const char *input)
    {
        std::string out;
        boost::algorithm::unhex(input, std::back_inserter(out));
        return out;
    }
};
