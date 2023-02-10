# start-cpp-vcpkg

### 一个 C++ 跨平台脚手架项目，使用 vcpkg + cmake 搭建：
- zlib: 最流行的数据压缩库。
- libcurl: 最流行的多协议文件传输库，支持 HTTP 请求。
- gflags: Google 命令行标志库。
- glog: Google 日志库。
- googletest: Google 测试框架。
- libuv: node.js 跨平台异步I/O，支持 TCP/UDP sockets。
- protobuf: Google 数据序列化库。
- sqlite3: 最流行的嵌入式关系数据库。
- cryptopp: 密码学库，支持 base64/RSA/AES 等常用加密。
- jsoncpp: json 序列化库。

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

### 查询 & 添加：
- `.\vcpkg\vcpkg.exe search xxx`
- `.\vcpkg\vcpkg.exe install xxx` or `vcpkg.json`

### Windows
- Windows 7 或更新的版本
- Visual Studio 2015 Update 3 或更新的版本（包含英文语言包）
- 运行 `bootstrap-vcpkg.bat` 脚本

### Unix
- g++ >= 6
- macOS:
  - `xcode-select --install`
  - `brew install pkg-config`
- Debian，Ubuntu，popOS 或其他基于 Debian 的发行版:
  - `sudo apt-get update`
  - `sudo apt-get install build-essential tar curl zip unzip`
- CentOS:
  - `sudo yum install centos-release-scl`
  - `sudo yum install devtoolset-7`
  - `scl enable devtoolset-7 bash`
- 运行 `bootstrap-vcpkg.sh` 脚本
