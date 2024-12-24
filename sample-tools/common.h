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
#include <iomanip>
#include <climits>
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

    /* boost UUID v4 (xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx)
     * 由32个十六进制字符组成，共有 2^128 ≈ 10^38 个，远远超过了宇宙中的粒子数量
     * 冲突概率可以忽略不计，参考生日悖论（Birthday Paradox）
     */
    static std::string GenUuid()
    {
        boost::uuids::random_generator gen;
        return boost::uuids::to_string(gen());
    }

    /* 仅使用 STL 实现的 UUID v4
     * 伪随机数生成器质量不高，会导致一定概率出现冲突（0.1% 以内）
     */
    static std::string GenUuid2()
    {
        // 使用 std::mt19937_64 随机数生成器
        std::random_device rd;                                      // 用于生成种子
        std::mt19937_64 gen(rd());                                  // 使用 64 位的随机数生成器
        std::uniform_int_distribution<uint64_t> dis(0, UINT64_MAX); // 生成 64 位范围的随机数

        // 生成两个 64 位的随机数，组成 128 位
        uint64_t part1 = dis(gen); // 生成第一个 64 位的随机数
        uint64_t part2 = dis(gen); // 生成第二个 64 位的随机数

        // 将两个部分合并成一个 128 位的随机数
        uint64_t time_low = part1;
        uint64_t time_mid = part2 & 0xFFFF;                    // 保证中间的 16 位是随机的
        uint64_t time_hi_and_version = (part2 >> 16) & 0x0FFF; // 高 12 位用于版本信息，设置为 `4`
        time_hi_and_version |= 0x4000;                         // 设置版本号为 `4`

        // 变体字段的高 2 位应为 `10`
        uint64_t clock_seq = dis(gen) & 0x3FFF; // 生成 14 位随机数
        clock_seq |= 0x8000;                    // 设置变体为 `10`，即高 2 位为 `10`

        uint64_t node = dis(gen); // 节点部分（48 位）

        // 格式化 UUID v4 输出
        std::stringstream ss;
        ss << std::hex << std::setfill('0')
           << std::setw(8) << (time_low)
           << "-"
           << std::setw(4) << (time_mid)
           << "-"
           << std::setw(4) << (time_hi_and_version)
           << "-"
           << std::setw(4) << (clock_seq)
           << "-"
           << std::setw(12) << (node);

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
