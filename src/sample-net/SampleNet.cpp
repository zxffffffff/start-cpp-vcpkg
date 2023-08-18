/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#include "SampleNet.h"
#include "uv.h"
#include "curl/curl.h"

SampleNet::SampleNet()
{
    std::cout << "libuv version: " << uv_version_string() << std::endl;
    std::cout << "curl version: " << curl_version() << std::endl;
}
