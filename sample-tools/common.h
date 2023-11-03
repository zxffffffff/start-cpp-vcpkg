/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#include <string>
#include <iostream>
#include <stdio.h>

#ifdef _WIN32
#include <conio.h>
#elif __APPLE__
#include <termios.h>
#include <unistd.h>
#endif

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable:4566)
#endif

#ifdef _WIN32
char cli_get_char()
{
    return getch();
}
#elif __APPLE__
char cli_get_char()
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
std::string inputPassword()
{
    std::string password;
    while (true)
    {
        char c = cli_get_char();
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
