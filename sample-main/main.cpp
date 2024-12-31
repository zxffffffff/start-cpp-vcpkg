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
#include "hardware.h"
#include "../version.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable : 4566)
#endif

int main(int argc, char *argv[])
{
#ifdef _WIN32
    /* 命令行统一 utf-8 (win7 不支持) */
    SetConsoleOutputCP(CP_UTF8);
#endif // _WIN32

    gflags::ParseCommandLineFlags(&argc, &argv, true);
    google::InitGoogleLogging(argv[0]);

    std::cout << "你好 SampleMain" << std::endl;

    std::cout << "PROJECT_VERSION: " << PROJECT_VERSION << std::endl;
    std::cout << "BUILD_DATE: " << BUILD_DATE << std::endl;
    std::cout << "GIT_COMMIT: " << GIT_COMMIT << std::endl;

    std::cout << "系统版本: " << Platform::GetOSType() << " " << Platform::GetOSVersion() << std::endl;
    std::cout << "CPU逻辑核心: " << Hardware::GetCPUs() << std::endl;
    std::cout << "Mac地址组合: " << Hardware::GetMacAddr() << std::endl;

    SampleDynamicLib sample_dynamic_lib;
    return 0;
}
