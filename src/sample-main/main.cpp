// cpp-deps.cpp: 定义应用程序的入口点。
//

#include "main.h"
#include "SampleDataCenter.h"
#include "glog/logging.h"

using namespace std;

int main(int argc, char *argv[])
{
  google::InitGoogleLogging(argv[0]);
	cout << "Hello CMake." << endl;

  SampleDataCenter sample_datacenter;
	return 0;
}
