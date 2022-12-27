# cpp-deps-vcpkg

### 一个 C++ 跨平台脚手架项目，使用 vcpkg + cmake 搭建：
- 基础组件：glog、gtest、zlib
- 网络层：libuv、curl
- 数据层：protobuf、xml、json、sqlite3
- 应用层：boost

### 使用 cmake 的原因：
- 使用广泛，资料多
- 跨平台

### 使用 vcpkg 的原因：
- 大部分开源库都能够找到，可以很好的和 cmake 一起工作
- 微软背书，跨平台


# vcpkg

### 来源：
- https://github.com/microsoft/vcpkg

### 文档：
- https://github.com/microsoft/vcpkg/blob/master/README_zh_CN.md

### Windows
- Windows 7 或更新的版本
- Visual Studio 2015 Update 3 或更新的版本（包含英文语言包）

### Unix
- g++ >= 6
- macOS:
  - xcode-select --install
- Debian，Ubuntu，popOS 或其他基于 Debian 的发行版:
  - sudo apt-get update
  - sudo apt-get install build-essential tar curl zip unzip
- CentOS:
  - sudo yum install centos-release-scl
  - sudo yum install devtoolset-7
  - scl enable devtoolset-7 bash

### Commands
- vcpkg install [name]:x64-windows // 区分 Manifest Mode
- vcpkg integrate
- vcpkg list
- vcpkg remove
- vcpkg search [name]
- vcpkg update-baseline
- vcpkg version

