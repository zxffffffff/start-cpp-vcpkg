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
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fmt/format.h>

// for system
#ifdef _WIN32
#define _WINSOCKAPI_ /* Prevent inclusion of winsock.h in windows.h */
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

// for io
#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
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
        int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.size(), nullptr, 0);
        std::wstring wstr(size, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.size(), &wstr[0], size);
        size = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), nullptr, 0, nullptr, nullptr);
        std::string result(size, '\0');
        WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), &result[0], size, nullptr, nullptr);
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
        int size = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), nullptr, 0);
        std::wstring wstr(size, L'\0');
        MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), &wstr[0], size);
        size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.size(), nullptr, 0, nullptr, nullptr);
        std::string result(size, '\0');
        WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.size(), &result[0], size, nullptr, nullptr);
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
        // Windows 10 = 10.0.19041
        // Windows 11 = 10.0.22000
        // GitHub windows-latest = 10.0.20348
        return fmt::format("{}.{}.{}", major, minor, build);
#elif __APPLE__
        char osproductversion[64]{0};
        size_t osproductversion_size = 64;
        sysctlbyname("kern.osproductversion", osproductversion, &osproductversion_size, nullptr, 0);
        char osversion[64]{0};
        size_t osversion_size = 64;
        sysctlbyname("kern.osversion", osversion, &osversion_size, nullptr, 0);
        // macOS Sonoma 14.5 = 14.5(23F79)
        // GitHub macos-latest = 14.6.1(23G93)
        return fmt::format("{}({})", osproductversion, osversion);
#elif __linux__
        struct utsname uts;
        uname(&uts);
        // CentOS-7 = 3.10.0-1160.102.1.el7.x86_64
        // GitHub ubuntu-latest = 6.5.0-1025-azure
        return uts.release;
#endif
    }

#ifdef _WIN32
    static char GetInputChar()
    {
        return getch();
    }
#else
    static char GetInputChar()
    {
        struct termios oldt, newt;
        char c;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        c = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return c;
    }
#endif

    /* 请输入密码 */
    static std::string GetInputPassword()
    {
        std::string password;
        while (true)
        {
            char c = GetInputChar();
            if (c == 8 || c == 127)
            {
                /* backspace */
                if (password.empty())
                    continue;
                std::cout << "\b \b";
                password.pop_back();
            }
            else if (c == '\r' || c == '\n')
            {
                /* 回车 */
                std::cout << std::endl;
                return password;
            }
            else if (c >= 32 && c <= 126)
            {
                /* 合法字符（可打印字符） */
                std::cout << '*';
                password.push_back(c);
            }
        }
    };
};
