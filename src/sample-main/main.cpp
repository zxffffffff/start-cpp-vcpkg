#include "SampleDataCenter.h"
#include "gflags/gflags.h"
#include "glog/logging.h"

using namespace std;

int main(int argc, char *argv[])
{
    gflags::ParseCommandLineFlags(&argc, &argv, true); 
    google::InitGoogleLogging(argv[0]);

    cout << "Hello SampleMain." << endl;

    SampleDataCenter sample_datacenter;
    return 0;
}
