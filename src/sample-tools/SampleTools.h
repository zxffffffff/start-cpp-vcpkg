#pragma once

#ifdef _WIN32
# ifndef SAMPLE_TOOLS_EXPORTS
#  define SAMPLE_TOOLS_API __declspec(dllexport)
# else
#  define SAMPLE_TOOLS_API __declspec(dllimport)
# endif
#else
# define SAMPLE_TOOLS_API __attribute__((visibility("default")))
#endif

#include <iostream>
#include <string.h>
#include <cassert>
#include <thread>

class SAMPLE_TOOLS_API SampleTools
{
public:
    SampleTools();
};
