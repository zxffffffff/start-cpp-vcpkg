#pragma once

#ifdef _WIN32
# ifndef SAMPLE_DATACENTER_EXPORTS
#  define SAMPLE_DATACENTER_API __declspec(dllexport)
# else
#  define SAMPLE_DATACENTER_API __declspec(dllimport)
# endif
#else
# define SAMPLE_DATACENTER_API __attribute__((visibility("default")))
#endif

#include <iostream>

class SAMPLE_DATACENTER_API SampleDataCenter
{
public:
    SampleDataCenter();
};
