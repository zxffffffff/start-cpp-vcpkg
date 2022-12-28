# start-cpp-vcpkg

### 一个 C++ 跨平台脚手架项目，使用 vcpkg + cmake 搭建：
- 基础组件：gflags、glog、gtest、zlib
- 网络层：libuv、curl
- 数据层：protobuf、xml、json、sqlite3
- 应用层：boost

### 快速开始：
- 运行 build-platform.xxx 在线安装第三方库，需要能够访问外网（github）
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

### Commands
- vcpkg install [name]:x64-windows //区分 Manifest Mode
- vcpkg integrate
- vcpkg list
- vcpkg remove
- vcpkg search [name]
- vcpkg update-baseline
- vcpkg version
