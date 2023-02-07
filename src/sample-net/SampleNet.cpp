#include "SampleNet.h"
#include "uv.h"
#include "curl/curl.h"

SampleNet::SampleNet()
{
    std::cout << "Hello SampleNet." << std::endl;

    std::cout << "libuv version: " << uv_version_string() << std::endl;
    
    std::cout << "curl version: " << curl_version() << std::endl;
}

int SampleNet::Test(int ret)
{
    return ret;
}
