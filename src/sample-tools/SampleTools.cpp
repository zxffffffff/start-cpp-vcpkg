/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/

#include "SampleTools.h"
#include "zlib.h"
#include "glog/logging.h"
#include "gflags/gflags.h"
#include "sqlite3.h"
#include "cryptopp/cryptlib.h"
#include "json/version.h"
#include "json/json.h"
#include <boost/algorithm/string.hpp>

SampleTools::SampleTools()
{
    std::cout << "Hello SampleTools." << std::endl;

    std::cout << "zlib version: " << zlibVersion() << std::endl;
    {
        unsigned char raw[64] = "abc123!@#$%^&*()_-+=*/\\'\"?你好";
        unsigned char dst[1024] = "";
        unsigned long dst_len = 0;
        compress(dst, &dst_len, raw, 64);

        unsigned char raw2[1024] = "";
        unsigned long raw2_len = 0;
        uncompress(raw2, &raw2_len, dst, dst_len);

        assert(strncmp((const char *)raw, (const char *)raw2, raw2_len) == 0);
    }

    std::cout << "gflags" << std::endl;
    {
        std::cout << "GetArgv: " << google::GetArgv() << std::endl;
    }

    std::cout << "glog" << std::endl;
    {
        assert(google::IsGoogleLoggingInitialized());
        LOG(INFO) << "This is INFO";
        LOG(WARNING) << "This is WARNING";
        LOG(ERROR) << "This is ERROR";
        // LOG(FATAL) << "This is FATAL";
    }

    std::cout << "sqlite3 version: " << sqlite3_libversion() << std::endl;

    std::cout << "cryptopp version: " << CryptoPP::HeaderVersion() << std::endl;
    {
        assert(CryptoPP::LibraryVersion() == CryptoPP::HeaderVersion());
    }

    std::cout << "jsoncpp version: " << JSONCPP_VERSION_STRING;
    {
        const std::string rawJson = R"({"Age": 28, "Name": "zxffffffff"})";
        const auto rawJsonLength = static_cast<int>(rawJson.length());
        constexpr bool shouldUseOldWay = false;
        JSONCPP_STRING err;
        Json::Value root;

        Json::CharReaderBuilder builder;
        const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
        if (reader->parse(rawJson.c_str(), rawJson.c_str() + rawJsonLength, &root, &err))
        {
            const std::string name = root["Name"].asString();
            const int age = root["Age"].asInt();
            assert(name == "zxffffffff");
            assert(age == 28);
        }
        else
        {
            assert(false);
        }
    }
}

std::string SampleTools::to_lower(const std::string& str)
{
    std::string s = str;
    boost::algorithm::to_lower(s);
    return s;
}
