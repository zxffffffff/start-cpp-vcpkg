# start-cpp-vcpkg

## 温馨提示：
- 不是所有的库都支持vcpkg的！
- 不是所有支持vcpkg的库都能编译过的！
- 该手动引入third-party的库还得手动来！
- 遇到问题提个Issue，微软会很快处理！
- 默认使用最新的库，如果需要指定旧版库，会很麻烦！

# 一个 C++ 跨平台脚手架项目，使用 vcpkg + cmake 搭建：
- sample-db
  - mysql-connector-cpp: 连接 MySQL 数据库。
  - mongo-cxx-driver: 连接 MongoDB 数据库。
  - hiredis: 连接 Redis 数据库。
- sample-pb
  - protobuf: Google 数据序列化库。
- sample-net (tcp, http)
  - ![image](https://github.com/zxffffffff/start-cpp-vcpkg/blob/master/sample-net/doc/states.png)
  - libcurl: 最流行的多协议文件传输库，支持 HTTP 请求。
  - libuv: node.js 跨平台异步I/O，支持 TCP/UDP sockets。
- sample-tools (crypto, threadpool)
  - gflags: Google 命令行标志库。
  - glog: Google 日志库。
  - boost: 标准库，可以按需单独安装某个库。
  - zlib: 最流行的数据压缩库。
  - sqlite3: 最流行的嵌入式关系数据库。
  - cryptopp: 密码学库，支持 base64/RSA/AES 等常用加密算法。
  - rapidjson: 腾讯出品，高性能 JSON 解析/生成器，灵感来自 RapidXml。
  - nlohmann-json: 现代的 JSON 解析/生成器，语法糖非常方便。
  - tinyxml: 轻量的 XML 解析库。
  - fmt: 格式化库，实现 C++20 std::format，完美取代 iostreams 和 printf。
- sample-test
  - googletest: Google 测试框架。

## 快速开始：
- 运行 build-platform.xxx 在线安装第三方库，需要能够访问外网（github）
- 网络异常导致的错误，可以尝试重新运行脚本（下载较多，可能需要反复重试）
- Windows 优先使用静态库，UNIX 和 APPLE 优先使用动态库，可以减少很多编译烦恼（编译问题99%来自于链接，运行问题99%来自于空/野指针）
- 跨平台 C++ 工程建议使用 UTF-8，这样仅需设置 Windows 编译环境（Windows 源码采用 GBK 编码，UNIX 源码采用 UTF-8 no BOM）
- 已验证：
  - Windows 10 x64
  - Ubuntu 20.04 x64
  - macOS 13.1 Apple M1
- 建议开发环境：
  - Visual Studio 2022 - Windows
  - Visual Studio Code - macOS、Linux
- 建议编译器：
  | 编译器        | 最低版本     |
  | ------------- | ------------ |
  | MSVC v143     | Windows 7    |
  | Xcode 12      | macOS 10.15  |
  | Clang 10      | Ubuntu 18.04 |

## 已知问题
- macOS
  - "protobuf" 和 "mysql-connector-cpp" 存在冲突，注意避免同时引用
  - https://github.com/microsoft/vcpkg/issues/27873
- Windows
  - "min" "max" 等宏定义冲突，报错可以 #undef 解决
- 指定版本号
  - 必须使用 `submodule` 引入 vcpkg，否则找不到 `.git` 文件
  - 查看历史版本：`git blame -l versions/l-/libuv.json`
  - 自动添加baseline：`.\vcpkg\vcpkg.exe x-update-baseline --add-initial-baseline`


# vcpkg 
- 安装 vcpkg 工具（已添加 submodule）
- 建议使用 submodule 引入`git submodule add -f https://github.com/microsoft/vcpkg.git vcpkg`

## 参考
- https://github.com/microsoft/vcpkg
- https://github.com/microsoft/vcpkg/blob/master/README_zh_CN.md

## 查询 & 添加 & 查看
```Bash
.\vcpkg\vcpkg.exe search xxx
.\vcpkg\vcpkg.exe install xxx` or `vcpkg.json
.\vcpkg\packages\xxx\CONTROL
```

## Windows（已加入 build 脚本）
- Windows 7 或更新的版本
- Visual Studio 2015 Update 3 或更新的版本（包含英文语言包）
- 运行 `bootstrap-vcpkg.bat` 脚本

## Unix（已加入 build 脚本）
- g++ >= 6
- macOS:
  ```Bash
  xcode-select --install
  brew install pkg-config
  ```
- Debian，Ubuntu，popOS 或其他基于 Debian 的发行版:
  ```Bash
  sudo apt-get update
  sudo apt-get install build-essential tar curl zip unzip
  ```
- CentOS:
  ```Bash
  sudo yum install centos-release-scl
  sudo yum install devtoolset-7
  scl enable devtoolset-7 bash
  ```
- 运行 `bootstrap-vcpkg.sh` 脚本

## Triplet 可选编译配置集
- https://learn.microsoft.com/en-us/vcpkg/users/triplets
- 在经典模式下，您可以使用三元组名称限定包引用，例如 `zlib:x64-windows-static-md`
- 在Manifest模式下，可以在命令行传递 `vcpkg install --triplet=<triplet>`
- 使用 CMake，您可以设置 `set(VCPKG_TARGET_TRIPLET <triplet>)`
- 使用 MSBuild，您可以设置 `VcpkgTriplet`
```
vcpkg built-in triplets:（官方提供 triplets）
  x64-linux
  x64-windows
  x64-windows-static
  x86-windows
  arm64-windows
  x64-uwp
  x64-osx
  arm-uwp
vcpkg community triplets:（社区提供 triplets 未经过持续集成测试可能不兼容）
  wasm32-emscripten
  x64-xbox-scarlett-static
  ppc64le-linux
  x64-xbox-xboxone
  arm-linux-release
  arm64-osx-release
  x86-mingw-static
  arm64-ios
  x86-linux
  x64-xbox-xboxone-static
  x64-mingw-dynamic
  x64-uwp-static-md
  x64-windows-release
  x86-freebsd
  armv6-android
  x64-osx-release
  x64-linux-dynamic
  x64-android
  riscv64-linux
  x86-windows-static
  arm64ec-windows
  arm-android
  arm64-mingw-dynamic
  arm64-uwp-static-md
  arm64-osx
  arm64-windows-static
  arm-linux
  arm-windows
  arm64-uwp
  x86-mingw-dynamic
  x86-uwp-static-md
  arm-uwp-static-md
  s390x-linux
  x64-openbsd
  arm-mingw-dynamic
  arm-neon-android
  x64-ios
  x64-xbox-scarlett
  x64-mingw-static
  arm-ios
  x64-osx-dynamic
  x64-linux-release
  x86-android
  x86-uwp
  arm64-linux
  x64-windows-static-release
  x64-freebsd
  arm64-linux-release
  riscv64-linux-release
  arm64-windows-static-md
  arm64-mingw-static
  x86-windows-static-md
  arm-mingw-static
  riscv32-linux-release
  arm64-osx-dynamic
  riscv32-linux
  x86-ios
  x64-windows-static-md
  arm64-windows-static-release
  arm64-android
  arm-windows-static
  s390x-linux-release
  ppc64le-linux-release
  x86-windows-v120
```
