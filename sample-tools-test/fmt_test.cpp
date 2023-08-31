/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#include "gtest/gtest.h"

#include <fmt/core.h>
#include <fmt/ranges.h>
#include <fmt/chrono.h>
#include <fmt/color.h>

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable:4566)
#endif

using namespace std::literals::chrono_literals;

TEST(fmt, test)
{
    fmt::print("Hello, world!\n");
    fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "Hello, {}!\n", "world");
    fmt::print(fg(fmt::color::floral_white) | bg(fmt::color::slate_gray) | fmt::emphasis::underline, "Hello, {}!\n", "мир");
    fmt::print(fg(fmt::color::steel_blue) | fmt::emphasis::italic, "Hello, {}!\n", "世界");

    std::string s = fmt::format("The answer is {}.", 42);
    EXPECT_EQ(s, "The answer is 42.");

    std::string s2 = fmt::format("I'd rather be {1} than {0}.", "right", "happy");
    EXPECT_EQ(s2, "I'd rather be happy than right.");

    std::vector<int> v = { 1, 2, 3 };
    std::string s3 = fmt::format("{}", v);
    EXPECT_EQ(s3, "[1, 2, 3]");

    std::string s4 = fmt::format("Default format: {} {}", 42s, 100ms);
    EXPECT_EQ(s4, "Default format: 42s 100ms");

    std::string s5 = fmt::format("strftime-like format: {:%H:%M:%S}", 3h + 15min + 30s);
    EXPECT_EQ(s5, "strftime-like format: 03:15:30");
}
