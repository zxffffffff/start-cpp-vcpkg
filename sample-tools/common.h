/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable : 4566)
#endif

std::string stringToHex(const std::string &input)
{
    std::stringstream ss;
    ss << std::hex << std::setfill('0');

    for (unsigned char c : input)
    {
        ss << std::setw(2) << static_cast<unsigned int>(c);
    }

    return ss.str();
}

std::string hexToString(const std::string &input)
{
    std::string output;

    for (size_t i = 0; i < input.length(); i += 2)
    {
        std::istringstream iss(input.substr(i, 2));
        int hexValue;
        iss >> std::hex >> hexValue;
        output += static_cast<char>(hexValue);
    }

    return output;
}
