# start-cpp-vcpkg

![workflow](https://github.com/zxffffffff/start-cpp-vcpkg/actions/workflows/build-windows.yml/badge.svg?event=push)
![workflow](https://github.com/zxffffffff/start-cpp-vcpkg/actions/workflows/build-macos.yml/badge.svg?event=push)
![workflow](https://github.com/zxffffffff/start-cpp-vcpkg/actions/workflows/build-ubuntu.yml/badge.svg?event=push)

从模板创建仓库: [start-cpp-vcpkg-template](https://github.com/zxffffffff/start-cpp-vcpkg-template)

## 温馨提示

- 可以使用环境变量 VCPKG_ROOT、VCPKG_DOWNLOADS 指定安装、下载目录，也可以每个项目自带一个 vcpkg 子模块
- 下载时网络不是很稳定，经常失败需要反复手动下载，建议仅开源项目和小型项目使用
- 叫得出名字的库基本都支持，如果 vcpkg search 没有那就真没有了

## 一个 C++ 跨平台脚手架项目，使用 vcpkg + cmake 搭建

- sample-tools
  - `gflags`: Google 命令行标志库。
  - `glog`: Google 日志库。
  - `boost`: 标准库，可以按需单独安装某个库。
  - `zlib`: 最流行的数据压缩库。
  - `snappy`: Google 快速压缩库。
  - `sqlite3`: 最流行的嵌入式关系数据库。
  - `cryptopp`: 密码学库，支持 base64/RSA/AES 等常用加密算法。
  - `rapidjson`: 腾讯出品，高性能 JSON 解析/生成器，灵感来自 RapidXml。
  - `nlohmann-json`: 现代的 JSON 解析/生成器，语法糖非常方便。
  - `tinyxml`: 轻量的 XML 解析库。
  - `fmt`: 格式化库，实现 C++20 std::format，完美取代 iostreams 和 printf。
- sample-db
  - `mysql-connector-cpp`: 连接 MySQL 数据库。
  - `mongo-cxx-driver`: 连接 MongoDB 数据库。
  - `hiredis`: 连接 Redis 数据库。
- sample-pb
  - `protobuf`: Google 数据序列化库。
- sample-net
  - `libcurl`: 最流行的多协议文件传输库，支持 HTTP 请求。
  - `libuv`: node.js 跨平台异步I/O，支持 TCP/UDP sockets。
- sample-test
  - `gtest`: Google 测试框架。

## 快速开始

- 运行 `build-xxx` 在线安装第三方库，需要能够访问外网 (github)
- 网络异常导致的错误，可以尝试重新运行脚本(下载较多，可能需要反复重试)
- 跨平台项目建议使用 静态库 (Linux默认静态编译，动态链接系统库)
- 跨平台项目建议使用 UTF-8 编码格式，这样仅需设置 Windows 编译环境 (Windows 中文系统默认使用 GBK 编码)
- 跨平台项目建议使用 `.gitattributes` 确保提交时转换为 `LF`

### 已验证

- macOS 14.5 (arm64-osx)
- Windows 10 (x64-windows)
- Ubuntu 24.04 (x64-linux)

### 编译器最低版本建议

| C++ compiler  | Minimum OS version |
| ------------- | ------------------ |
| MSVC v140     | Windows 7          |
| Clang 12      | macOS 10.15        |
| GCC 7.3       | Ubuntu 18.04       |

### CMake CXX_STANDARD 版本参考

| Added in version  | C++ Standard    |
| ----------------- | --------------- |
| 3.1               | C++ 98, 11, 14  |
| 3.8               | C++ 17          |
| 3.12              | C++ 20          |
| 3.20              | C++ 23          |
| 3.25              | C++ 26          |

### Windows MSVC 参考

| CMake version         | MSVC version    |
| --------------------- | --------------- |
| Visual Studio 17 2022 | v143            |
| Visual Studio 16 2019 | v142            |
| Visual Studio 15 2017 | v141            |
| Visual Studio 14 2015 | v140            |
| Visual Studio 12 2013 | 不保证二进制兼容性 |

### Apple C++ 版本参考

| C++ Feature        | Minimum deployment target                     |
| ------------------ | --------------------------------------------- |
| C++ 17 Filesystem  | macOS 10.15, iOS 13.0, tvOS 13.0, watchOS 6.0 |

### Linux Glibc 版本参考

| Glibc | Ubuntu             | CentOS         | Debian          |
| ----- | ------------------ | -------------- | --------------- |
| 2.35  | 22.04 (GCC 11.2.0) |                |                 |
| 2.34  |                    | 9 (GCC 11.2.1) |                 |
| 2.31  | 20.04 (GCC 9.3.0)  |                | 11 (GCC 10.2.1) |
| 2.28  |                    | 8 (GCC 8.3.1)  | 10 (GCC 8.3.0)  |
| 2.27  | 18.04 (GCC 7.3.0)  |                |                 |
| 2.24  |                    |                | 9 (GCC 6.3.0)   |
| 2.23  | 16.04 (GCC 5.4.0)  |                |                 |
| 2.19  | 14.04 (GCC 4.8.2)  |                | 8 (GCC 4.9.2)   |
| 2.17  |                    | 7 (GCC 4.8.5)  |                 |
| 2.15  | 12.04 (GCC 4.6.3)  |                |                 |
| 2.13  |                    |                | 7 (GCC 4.7.2)   |
| 2.12  |                    | 6 (GCC 4.4.7)  |                 |

## 注意事项

### Windows 宏定义冲突

- `min` `max` 等冲突报错可以 `#undef` 解决

### Linux 打包

- 使用 `-static-libgcc -static-libstdc++` 静态编译 C++
- 不能使用 `-static` 静态编译 C，需要注意开发环境使用低版本 `glibc` 编译动态链接

### Protobuf 静态链接

- 注意编译 `.proto` 文件必须使用 `libprotobuf` 相同版本
- Windows 下静态链接 `.pb.cc` 等文件需要静态编译 `libprotobuf`

### vcpkg 指定编译器(降级)

- 除了使用 `cmake -G` 之外，还需要在 `vcpkg/triplets/xxx.cmake` 指定编译器，否则默认使用已安装的最新版本
- 手动修改 triplets 配置，例如设置 MSVC 版本 `set(VCPKG_PLATFORM_TOOLSET v140)`

### vcpkg 指定版本号(降级)

- 必须使用 `git clone` 或 `submodule` 引入 vcpkg，`subtree` 会报错找不到 `.git` 文件
- 查看历史版本：`git blame -l versions/l-/libuv.json`
- 自动添加baseline：`vcpkg x-update-baseline --add-initial-baseline`

```js
// libuv 1.41 是最后一个支持 win7 的版本
"overrides": [
  {
    "name": "libuv",
    "version": "1.41.0",
    "port-version": 1
  }
],
"builtin-baseline": "b051745c68faa6f65c493371d564c4eb8af34dad"
```

# Vcpkg

- 安装 `vcpkg` 工具 (参考本工程 `.gitmodules`)
- 手动引入建议使用 `git submodule add -f https://github.com/microsoft/vcpkg.git vcpkg`
- 初始化、拉取更新可以使用 `git submodule update --init --recursive`

## 参考

- <https://github.com/microsoft/vcpkg>
- <https://github.com/microsoft/vcpkg/blob/master/README_zh_CN.md>

## 查询 & 添加 & 查看

```Bash
.\vcpkg\vcpkg.exe search xxx
.\vcpkg\vcpkg.exe install xxx` or `vcpkg.json
.\vcpkg\packages\xxx\CONTROL
```

## Windows

- Windows 7 或更新的版本
- Visual Studio 2015 Update 3 或更新的版本 (包含英文语言包)
- 运行 `bootstrap-vcpkg.bat` 脚本

## Unix

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

- <https://learn.microsoft.com/en-us/vcpkg/users/triplets>
- 在经典模式下，您可以使用三元组名称限定包引用，例如 `zlib:x64-windows-static-md`
- 在Manifest模式下，可以在命令行传递 `vcpkg install --triplet=<triplet>`
- 使用 CMake，您可以设置 `set(VCPKG_TARGET_TRIPLET <triplet>)`
- 查看三元组 `vcpkg help triplet`
- 可以手动修改 triplets 配置

```
Built-in Triplets:
  x64-android
  x64-linux
  arm64-osx
  x64-windows
  arm64-uwp
  x64-windows-static
  x86-windows
  arm-neon-android
  arm64-windows
  x64-uwp
  x64-osx
  arm64-android
Community Triplets:
  wasm32-emscripten
  x64-xbox-scarlett-static
  ppc64le-linux
  arm64-ios-simulator-release
  x64-xbox-xboxone
  arm-linux-release
  arm64-osx-release
  x86-mingw-static
  arm64-ios
  x86-linux
  arm64-ios-release
  x64-xbox-xboxone-static
  x64-mingw-dynamic
  x64-uwp-static-md
  x64-windows-release
  loongarch32-linux
  x86-freebsd
  armv6-android
  x64-osx-release
  x64-linux-dynamic
  riscv64-linux
  loongarch32-linux-release
  x86-windows-static
  mips64-linux
  arm64ec-windows
  arm-android
  arm64-mingw-dynamic
  arm64-uwp-static-md
  arm64-windows-static
  arm-linux
  arm-windows
  loongarch64-linux-release
  x86-mingw-dynamic
  x86-uwp-static-md
  arm-uwp-static-md
  s390x-linux
  x64-openbsd
  arm-mingw-dynamic
  x64-ios
  x64-xbox-scarlett
  loongarch64-linux
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
  arm64-ios-simulator
  arm-mingw-static
  riscv32-linux-release
  arm64-osx-dynamic
  x64-windows-static-md-release
  riscv32-linux
  x86-ios
  x64-windows-static-md
  arm64-windows-static-release
  arm-windows-static
  s390x-linux-release
  ppc64le-linux-release
  x86-windows-v120
  arm-uwp
```
