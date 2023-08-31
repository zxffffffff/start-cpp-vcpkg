/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include <string>

#ifdef _WIN32
#include <windows.h>
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
        return result;
#else
        return str;
#endif
    }
};
