#include "SampleTools.h"
#include "zlib.h"
#include "glog/logging.h"

SampleTools::SampleTools()
{
    std::cout << "Hello SampleTools." << std::endl;

    std::cout << "zlib version: " << zlibVersion() << std::endl;
    {
        unsigned char raw[64] = "aaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbcccccccccccccdddddddd";
        unsigned char dst[1024] = "";
        unsigned long dst_len = 0;
        compress(dst, &dst_len, raw, 64);

        unsigned char raw2[1024] = "";
        unsigned long raw2_len = 0;
        uncompress(raw2, &raw2_len, dst, dst_len);

        assert(strncmp((const char*)raw, (const char*)raw2, raw2_len) == 0);
    }

    std::cout << "glog" << std::endl;
    {
        assert(google::IsGoogleLoggingInitialized());
        LOG(INFO) << "This is INFO";
        LOG(WARNING) << "This is WARNING";
        LOG(ERROR) << "This is ERROR";
        //LOG(FATAL) << "This is FATAL";
    }
}
