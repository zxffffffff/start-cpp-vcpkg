/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include "cpp_version.h"
#include <memory>
#include <fmt/format.h>
#include <gflags/gflags.h>
#include <glog/logging.h>

#if CPP_VERSION >= 2017
#include <filesystem>
#endif

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable : 4566)
#endif

/* TODO 最好再封装一层glog参数 */
// DEFINE_int32(logCleaner, 15, "自动删除旧日志");

#define LOG_HEAD (__func__)

class Log
{
public:
    static Log &Singleton()
    {
        static Log instance;
        return instance;
    }

private:
    Log() {}

    std::atomic<bool> app_exit{false};

public:
    static void Init(int argc, char *argv[])
    {
        /* 确保指针安全 */
        static std::vector<std::string> argvCopy(argc);
        static std::vector<char *> argvCopy2(argc);
        for (int i = 0; i < argc; ++i)
        {
            argvCopy[i] = argv[i];
            argvCopy2[i] = (char *)argvCopy[i].data();
        }
        char **argv2 = argvCopy2.data();
        gflags::ParseCommandLineFlags(&argc, &argv2, false);

        /* 日志路径
         * log_dir (string, default="")
         * 如果指定，日志文件将写入此目录而不是默认日志记录目录。
         */
        std::string logPath = FLAGS_log_dir;
        if (logPath.empty())
            logPath = "./log";
#if CPP_VERSION >= 2017
        if (!std::filesystem::exists(logPath))
            std::filesystem::create_directory(logPath);
#endif

        /* 日志级别
         * minloglevel (int, default=0, which is INFO)
         * 记录处于或高于此级别的消息。同样，严重级别 INFO 、 WARNING 、 ERROR 和 FATAL 的数量分别为 0、1、2 和 3 。
         */
        int logLevel = FLAGS_minloglevel;

        /* 按Level设置路径
         * 如果base_filename是""，它意味着"不要记录此级别"。
         */
        if (logLevel > google::GLOG_INFO)
            google::SetLogDestination(google::GLOG_INFO, "");
        if (logLevel > google::GLOG_WARNING)
            google::SetLogDestination(google::GLOG_WARNING, "");
        if (logLevel > google::GLOG_ERROR)
            google::SetLogDestination(google::GLOG_ERROR, "");

        /* 自动删除旧日志
         * 然后每次执行flush时glog都会检查是否有过期的日志。
         */
        // int logCleaner = FLAGS_logCleaner;
        // if (logCleaner > 0)
        //     google::EnableLogCleaner(logCleaner); // days

        /* 消息同时输出 stderr
         * Set whether log messages go to stderr in addition to logfiles.
         */
        bool logToConsole = FLAGS_alsologtostderr;

        /* log后缀方便双击打开 */
        google::SetLogFilenameExtension(".log");

        /* 控制台警告颜色 */
        FLAGS_colorlogtostderr = true;

        /* 立即写入文件 */
        FLAGS_logbufsecs = 0;

        /* 磁盘写满后不继续写入 */
        // FLAGS_stop_logging_if_full_disk = true;

        /* TODO 仅分片写入日志，不会自动删除 */
        FLAGS_max_log_size = 200; // MB

        google::InitGoogleLogging(argv[0]);

        /* 确保 WARNING 以上也写日志 */
        if (FLAGS_minloglevel <= 1)
        {
            LOG(WARNING) << LOG_HEAD;
        }
        else
        {
            LOG(ERROR) << LOG_HEAD;
        }
    }

    /* 崩溃拦截日志 */
    static void InitCrashReport()
    {
        static bool init_once = false;
        if (init_once)
            return;
        init_once = true;

        auto failure_writer = [](const char *data, size_t size)
        {
            /* 退出时不记录 core dump */
            if (Singleton().app_exit)
                return;
            std::string log = fmt::format("{} {}", LOG_HEAD, std::string(data, size));
            LOG(ERROR) << log;
        };
        google::InstallFailureSignalHandler();
        google::InstallFailureWriter(failure_writer);
    }

    static void Cleanup()
    {
        /* 优雅退出 */
        Singleton().app_exit = true;

        LOG(WARNING) << LOG_HEAD;
        google::ShutdownGoogleLogging();
    }
};
