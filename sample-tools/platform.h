/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include <string>
#include <set>
#include <sstream>
#include <iomanip>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/algorithm/string.hpp>

#ifdef _WIN32
#include <windows.h>
#undef min
#undef max
#undef ERROR /* 和glog冲突 */
#elif __APPLE__
#include <sys/sysctl.h>
#elif __linux__
#include <sys/utsname.h>
#else
#error "Unknown compiler"
#endif

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable : 4566)
#endif

class Platform
{
public:
    /* utf-8 -> 本地编码（Windows:GB2312） */
    static std::string utf8_to_local(const std::string &str)
    {
#ifdef _WIN32
        if (str.empty())
            return str;

        int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
        std::wstring wstr(size, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size);
        size = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
        std::string result(size, '\0');
        WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, &result[0], size, nullptr, nullptr);

        while (result.size() && result.back() == '\0')
            result.pop_back();
        return result;
#else
        return str;
#endif
    }

    /* 本地编码（Windows:GB2312）-> utf-8 */
    static std::string local_to_utf8(const std::string &str)
    {
#ifdef _WIN32
        if (str.empty())
            return str;

        int size = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, nullptr, 0);
        std::wstring wstr(size, L'\0');
        MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, &wstr[0], size);
        size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
        std::string result(size, '\0');
        WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &result[0], size, nullptr, nullptr);

        while (result.size() && result.back() == '\0')
            result.pop_back();
        return result;
#else
        return str;
#endif
    }

    /* OS类型 */
    static std::string GetOSType()
    {
#ifdef _WIN32
        return "Windows";
#elif __APPLE__
        return "macOS";
#elif __linux__
        /* uname sysname 获取发行版名 */
        return "Linux";
#endif
    }

    /* OS版本号 */
    static std::string GetOSVersion()
    {
#ifdef _WIN32
        // KUSER_SHARED_DATA
        BYTE *sharedUserData = (BYTE *)0x7FFE0000;
        ULONG major = *(ULONG *)(sharedUserData + 0x26c);
        ULONG minor = *(ULONG *)(sharedUserData + 0x270);
        ULONG build = *(ULONG *)(sharedUserData + 0x260);
        return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(build);
#elif __APPLE__
        size_t size = 0;
        sysctlbyname("kern.osrelease", nullptr, &size, nullptr, 0);
        char *sysctlResult = new char[size];
        sysctlbyname("kern.osrelease", sysctlResult, &size, nullptr, 0);
        std::string version(sysctlResult);
        delete[] sysctlResult;
        return version;
#elif __linux__
        struct utsname uts;
        uname(&uts);
        // CentOS 7 "3.10.0-1160.102.1.el7.x86_64"
        return uts.release;
#endif
    }

    /* 随机guid (00000000-0000-0000-0000-000000000000) */
    static std::string GenGuid()
    {
        boost::uuids::random_generator gen;
        return boost::uuids::to_string(gen());
    }
};
