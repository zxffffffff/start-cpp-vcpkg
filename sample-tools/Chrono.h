/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include <memory>
#include <chrono>

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable:4566)
#endif

class Chrono
{
public:
    Chrono(bool _start = true)
    {
        if (_start)
            start();
    }

    ~Chrono() = default;

    void start()
    {
        time_start = std::chrono::steady_clock::now();
    }

    int64_t stop()
    {
        time_stop = std::chrono::steady_clock::now();
        return use_time();
    }

    int64_t use_time()
    {
        return std::chrono::duration<double, std::milli>(time_stop - time_start).count();
    }

private:
    std::chrono::steady_clock::time_point time_start, time_stop;
};
