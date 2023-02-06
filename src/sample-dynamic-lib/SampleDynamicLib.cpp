#include "SampleDynamicLib.h"
#include "SampleDataCenter.h"

SampleDynamicLib::SampleDynamicLib()
{
    std::cout << "Hello SampleDynamicLib." << std::endl;

    SampleDataCenter sample_datacenter;
}

int SampleDynamicLib::Test(int ret)
{
    return ret;
}
