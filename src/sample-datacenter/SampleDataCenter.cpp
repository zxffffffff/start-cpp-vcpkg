/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/

#include "SampleDataCenter.h"
#include "SampleTools.h"
#include "SampleDB.h"
#include "SamplePB.h"
#include "SampleNet.h"

SampleDataCenter::SampleDataCenter()
{
    std::cout << "Hello SampleDataCenter." << std::endl;

    SampleTools sample_tools;
    SampleDB sample_db;
    SamplePB sample_pb;
    SampleNet sample_net;
}

int SampleDataCenter::Test(int ret)
{
    return ret;
}
