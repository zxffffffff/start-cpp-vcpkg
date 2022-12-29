#include "SampleDataCenter.h"
#include "SampleTools.h"
#include "SamplePB.h"
#include "SampleNet.h"

SampleDataCenter::SampleDataCenter()
{
    std::cout << "Hello SampleDataCenter." << std::endl;

    SampleTools sample_tools;
    SamplePB sample_pb;
    SampleNet sample_net;
}

int SampleDataCenter::Test(int ret)
{
    return ret;
}
