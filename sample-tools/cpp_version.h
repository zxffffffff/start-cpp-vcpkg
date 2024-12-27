/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include <string>

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable : 4566)
#endif

#if __cplusplus >= 202302L || _MSVC_LANG >= 202302L /* C++23 */
#define CPP_VERSION 2023
#elif __cplusplus >= 202002L || _MSVC_LANG >= 202002L /* C++20, C++2a */
#define CPP_VERSION 2020
#elif __cplusplus >= 201703L || _MSVC_LANG >= 201703L /* C++17, C++1z */
#define CPP_VERSION 2017
#elif __cplusplus >= 201402L || _MSVC_LANG >= 201402L /* C++14, C++1y */
#define CPP_VERSION 2014
#elif __cplusplus >= 201103L /* C++11, C++0x */
#define CPP_VERSION 2011
#elif __cplusplus >= 199711L /* C++03, C++98 (until C++11) */
#define CPP_VERSION 1997
#else
#error unknown version of C++ standard
#endif

#if CPP_VERSION <= 2011
#include <memory>

namespace std
{
    /* 兼容C++14 make_unique */
    template <typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args &&...args)
    {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
}
#endif // C++ 11
