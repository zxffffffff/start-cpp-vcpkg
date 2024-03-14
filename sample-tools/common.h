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
#include <chrono>
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
    /* 纪元时间（Unix Epoch Time）1970 至今 */
    static int64_t NowSinceEpoch_MS()
    {
        auto now = std::chrono::system_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    }

    /* 随机guid (00000000-0000-0000-0000-000000000000) */
    static std::string GenGuid()
    {
        boost::uuids::random_generator gen;
        return boost::uuids::to_string(gen());
    }

    /* 时间戳转换 YYYY-mm-dd HH:MM:SS.ms */
    static std::string TimeFormat(int64_t time_ms)
    {
        std::time_t time = time_ms / 1000;
        int64_t milliseconds = time_ms % 1000;

        std::ostringstream oss;
        std::tm *local_time = std::localtime(&time);
        oss << std::put_time(local_time, "%Y-%m-%d %H:%M:%S");
        oss << "." << std::setw(3) << std::setfill('0') << milliseconds;
        return oss.str();
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
