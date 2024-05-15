/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#include "gtest/gtest.h"
#include <gflags/gflags.h>
#include <glog/logging.h>
// #include <filesystem>
#include <iostream>

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable : 4566)
#endif

TEST(glog, init)
{
    static int argc = 3;
    static std::vector<std::string> s_argv(argc);
    static std::vector<char *> c_argv(argc);
    for (int i = 0; i < argc; ++i)
    {
        s_argv[i] = "argv_" + std::to_string(i);
        c_argv[i] = (char *)s_argv[i].data();
    }
    static char **argv = c_argv.data();

    gflags::ParseCommandLineFlags(&argc, &argv, true);

    // FLAGS_log_dir = "/path/to/log";
    // if (!std::filesystem::exists(FLAGS_log_dir))
    //     std::filesystem::create_directory(FLAGS_log_dir);
    google::EnableLogCleaner(15); // days
    // google::SetLogDestination /* 按级别设置路径 */
    google::SetLogFilenameExtension(".log");
    FLAGS_colorlogtostderr = true; // RGB
    FLAGS_max_log_size = 200;      // MB
    google::InitGoogleLogging(argv[0]);
    {
        const std::string filename = google::ProgramInvocationShortName();
        // LOG(ERROR) << "google::ProgramInvocationShortName() = " << filename;
    }
    google::ShutdownGoogleLogging();
}
