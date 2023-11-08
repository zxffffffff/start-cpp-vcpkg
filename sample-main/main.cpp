/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#include "gflags/gflags.h"
#include "glog/logging.h"
#include "SampleDynamicLib.h"
#include "platform.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable:4566)
#endif

int main(int argc, char *argv[])
{
#ifdef _WIN32
    /* 命令行统一 utf-8 */
    SetConsoleOutputCP(CP_UTF8);
#endif // _WIN32

    gflags::ParseCommandLineFlags(&argc, &argv, true);
    google::InitGoogleLogging(argv[0]);

    std::cout << "你好 SampleMain." << std::endl;

    SampleDynamicLib sample_dynamic_lib;
    return 0;
}
