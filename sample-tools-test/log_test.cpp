/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#include "gtest/gtest.h"
#include "log.h"
#include <boost/algorithm/string.hpp>

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable : 4566)
#endif

TEST(log, args)
{
    std::vector<char *> argvEx;
    // for (int i = 0; i < argc; ++i)
    //     argvEx.push_back(argv[i]);
    argvEx.push_back((char *)"app");

    std::string config_logDir = testing::TempDir();
    {
        static std::string log_dir = "--log_dir=" + config_logDir;
        argvEx.push_back((char *)log_dir.data());
    }

    std::string config_logLevel = "Warning";
    if (!config_logLevel.empty())
    {
        int level = 0;
        boost::algorithm::to_upper(config_logLevel);
        if (config_logLevel.size() == 1 && '0' <= config_logLevel[0] && config_logLevel[0] <= '3')
            level = std::atoi(config_logLevel.c_str());
        else if (config_logLevel == "INFO")
            level = 0;
        else if (config_logLevel == "WARNING")
            level = 1;
        else if (config_logLevel == "ERROR")
            level = 2;
        else if (config_logLevel == "FATAL")
            level = 3;
        static std::string minloglevel = "--minloglevel=" + std::to_string(level);
        argvEx.push_back((char *)minloglevel.data());
    }

    // std::string config_logCleaner = "7";
    // if (!config_logCleaner.empty())
    // {
    //     static std::string logCleaner = "--logCleaner=" + config_logCleaner;
    //     argvEx.push_back((char *)logCleaner.data());
    // }

    std::string config_logToConsole = "1";
    if (config_logToConsole == "1")
    {
        static std::string alsologtostderr = "--alsologtostderr";
        argvEx.push_back((char *)alsologtostderr.data());
    }
    else if (config_logToConsole == "2")
    {
        static std::string logtostderr = "--logtostderr";
        argvEx.push_back((char *)logtostderr.data());
    }

    Log::Init(argvEx.size(), argvEx.data());
    Log::InitCrashReport();

    auto test = []
    {
        for (int i = 0; i < 5; ++i)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            if (i == 0)
                LOG(INFO) << "Hello, " << i << "!";
            else if (i == 1)
                LOG(WARNING) << "Hello, " << i << "!";
            else
                LOG(ERROR) << "Hello, " << i << "!";
        }
    };
    std::thread thread(test);
    test();
    thread.join();

    Log::Cleanup();
}
