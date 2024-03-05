/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#include <string>
#include <iostream>
#include <sstream>
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
