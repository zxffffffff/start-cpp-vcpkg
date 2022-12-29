# start-cpp-vcpkg

### 一个 C++ 跨平台脚手架项目，使用 vcpkg + cmake 搭建：
- ZLib: 非常紧凑的数据流压缩库。
- libcurl: 多协议文件传输库。（HTTP）
- gflags: C++的命令行标志模块。
- glog: Google日志模块的C++实现。
- Google Test: Google的C++测试框架。
- Google Mock: 编写和使用C++模拟类的库。
- libuv: 跨平台异步I/O。（TCP/UDP）
- protobuf: 协议缓冲，谷歌的数据交换格式。
- SQLite: 一个完全嵌入式的，功能齐全的关系数据库，只有几百KB，可以正确包含到你的项目中。

### 快速开始：
- 运行 build-platform.xxx 在线安装第三方库，需要能够访问外网（github）
- 优先使用静态库，可以减少很多编译烦恼，延长寿命
- 已验证：
  - Windows 10 x64
  - Ubuntu 20.04 x64
  - macOS 13.1 Apple M1



# vcpkg

### 参考：
- https://github.com/microsoft/vcpkg
- https://github.com/microsoft/vcpkg/blob/master/README_zh_CN.md

### Windows
- Windows 7 或更新的版本
- Visual Studio 2015 Update 3 或更新的版本（包含英文语言包）
- 运行 bootstrap-vcpkg.bat 脚本

### Unix
- g++ >= 6
- macOS:
  - xcode-select --install
  - brew install pkg-config
- Debian，Ubuntu，popOS 或其他基于 Debian 的发行版:
  - sudo apt-get update
  - sudo apt-get install build-essential tar curl zip unzip
- CentOS:
  - sudo yum install centos-release-scl
  - sudo yum install devtoolset-7
  - scl enable devtoolset-7 bash
- 运行 bootstrap-vcpkg.sh 脚本
