#include "SampleDynamicLib.h"
#include "gflags/gflags.h"
#include "glog/logging.h"

using namespace std;

int main(int argc, char *argv[])
{
    gflags::ParseCommandLineFlags(&argc, &argv, true); 
    google::InitGoogleLogging(argv[0]);

    cout << "Hello SampleMain." << endl;

    SampleDynamicLib sample_dynamic_lib;
    return 0;
}
