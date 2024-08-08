/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include "cpp_version.h"
#include <string>
#include <iostream>
#include <sstream>
#include <chrono>
#include <random>
#include <thread>
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
    /* 更准确的 sleep，可能会提前 */
    static int64_t Sleep(int64_t ms)
    {
        auto start = std::chrono::high_resolution_clock::now();
        auto target_wake_time = start + std::chrono::milliseconds(ms);
        std::this_thread::sleep_until(target_wake_time);
        auto stop = std::chrono::high_resolution_clock::now();
        int64_t use = (stop - start).count();
        return use - ms;
    }

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

    /* 接近于 boost::uuids，小概率出现重复 */
    static std::string GenGuid2()
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 15);
        std::uniform_int_distribution<> dis2(8, 11);

        std::stringstream ss;
        ss << std::hex << std::setfill('0');

        for (int i = 0; i < 8; i++)
            ss << std::setw(1) << dis(gen);
        ss << "-";
        for (int i = 0; i < 4; i++)
            ss << std::setw(1) << dis(gen);
        ss << "-4"; // Version 4 UUID
        for (int i = 0; i < 3; i++)
            ss << std::setw(1) << dis(gen);
        ss << "-";
        ss << std::setw(1) << dis2(gen); // The first character should be between 8 and b
        for (int i = 0; i < 3; i++)
            ss << std::setw(1) << dis(gen);
        ss << "-";
        for (int i = 0; i < 12; i++)
            ss << std::setw(1) << dis(gen);
        return ss.str();
    }

    /* 伪随机数 */
    static int Random(int n_min = 0, int n_max = INT_MAX)
    {
        std::random_device r;
        std::default_random_engine el(r());
        std::uniform_int_distribution<int> uniform_dist(n_min, n_max);
        return uniform_dist(el);
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

    /* 线程安全的 static 缓冲区 */
    static const char *toThreadLocalStr(const std::string &str)
    {
        thread_local std::string buf;
        buf = str;
        return buf.data();
    }
};
