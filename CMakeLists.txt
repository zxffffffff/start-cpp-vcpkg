#############################################################################
## MIT License
## 
## Author	: xiaofeng.zhu
## Support	: zxffffffff@outlook.com, 1337328542@qq.com
## 
#############################################################################

message("[start-cpp-vcpkg] -------- start")
cmake_minimum_required(VERSION 3.7.2)
set(CMAKE_CXX_STANDARD 11)
set(CXX_STANDARD_REQUIRED ON)

if(WIN32)
    # 源码使用 utf-8 编码格式编译
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

    # 手动启用 __cplusplus 宏（否则始终是 199711L）
    # 手动启用 /permissive- 标准一致性模式（VS2017+）
    # 手动启用 /EHsc C++ 异常: https://learn.microsoft.com/zh-cn/cpp/build/reference/eh-exception-handling-model
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc /permissive- /Zc:__cplusplus")
elseif(APPLE)
    # 编译动态库需要设置-fPIC
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

    # 需要 macOS 10.15 支持 C++17 filesystem
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15")
else()
    # 编译动态库需要设置-fPIC
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

    # 静态链接 C++ 依赖库，不要使用 -static 静态链接 glibc
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
endif()

# 可选 vcpkg 编译配置集: https://learn.microsoft.com/en-us/vcpkg/users/triplets
if(NOT DEFINED VCPKG_TARGET_TRIPLET)
    if(WIN32)
        set(VCPKG_TARGET_TRIPLET "x64-windows")
    elseif(APPLE)
        set(VCPKG_TARGET_TRIPLET "arm64-osx")
    elseif(UNIX)
        set(VCPKG_TARGET_TRIPLET "x64-linux")
    else()
        message(FATAL_ERROR "Unsupported platform. Please define VCPKG_TARGET_TRIPLET manually.")
    endif()
endif()
message(STATUS "[start-cpp-vcpkg] VCPKG_TARGET_TRIPLET : ${VCPKG_TARGET_TRIPLET}")

# 使用 vcpkg 工具链，需要在 project 之前设置
if(DEFINED ENV{VCPKG_ROOT})
    # 使用 VCPKG_ROOT 环境变量
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
else()
    # 从 github 下载到根目录
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg")
        message(STATUS "Cloning vcpkg into ${CMAKE_CURRENT_SOURCE_DIR}")
        execute_process(
            COMMAND git clone https://github.com/microsoft/vcpkg.git "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg"
            RESULT_VARIABLE GIT_CLONE_RESULT
        )
        if(NOT GIT_CLONE_RESULT EQUAL "0")
            message(FATAL_ERROR "Failed to clone vcpkg repository.")
        endif()
    endif()
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
endif()
message(STATUS "[start-cpp-vcpkg] CMAKE_TOOLCHAIN_FILE : ${CMAKE_TOOLCHAIN_FILE}")

project("start-cpp-vcpkg")

message(STATUS "[start-cpp-vcpkg] CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
message(STATUS "[start-cpp-vcpkg] CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "[start-cpp-vcpkg] CMAKE_CURRENT_SOURCE_DIR : ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "[start-cpp-vcpkg] PROJECT_SOURCE_DIR : ${PROJECT_SOURCE_DIR}")

# 生成版本头文件 (项目版本、构建日期、Git记录)
set(PROJECT_VERSION "0.2.1")
execute_process(COMMAND date "+%Y-%m-%d %H:%M:%S" OUTPUT_VARIABLE BUILD_DATE OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND git describe --tags --always OUTPUT_VARIABLE GIT_COMMIT OUTPUT_STRIP_TRAILING_WHITESPACE)

message(STATUS "[start-cpp-vcpkg] PROJECT_VERSION: ${PROJECT_VERSION}")
message(STATUS "[start-cpp-vcpkg] BUILD_DATE: ${BUILD_DATE}")
message(STATUS "[start-cpp-vcpkg] GIT_COMMIT: ${GIT_COMMIT}")

file(WRITE version.h "#ifndef VERSION_H\n#define VERSION_H\n\n")
file(APPEND version.h "const char* PROJECT_VERSION = \"${PROJECT_VERSION}\";\n")
file(APPEND version.h "const char* BUILD_DATE = \"${BUILD_DATE}\";\n")
file(APPEND version.h "const char* GIT_COMMIT = \"${GIT_COMMIT}\";\n")
file(APPEND version.h "\n#endif /* VERSION_H */\n")

# src
include_directories(${PROJECT_SOURCE_DIR}/sample-tools)
include_directories(${PROJECT_SOURCE_DIR}/sample-pb)
include_directories(${PROJECT_SOURCE_DIR}/sample-net)
include_directories(${PROJECT_SOURCE_DIR}/sample-db)
include_directories(${PROJECT_SOURCE_DIR}/sample-datacenter)
include_directories(${PROJECT_SOURCE_DIR}/sample-dynamic-lib)
include_directories(${PROJECT_SOURCE_DIR}/sample-main)
add_subdirectory(sample-tools)
add_subdirectory(sample-pb)
add_subdirectory(sample-net)
add_subdirectory(sample-db)
add_subdirectory(sample-datacenter)
add_subdirectory(sample-dynamic-lib)
add_subdirectory(sample-main)

# test
include(CTest) # 支持 ctest 命令
include_directories(${PROJECT_SOURCE_DIR}/sample-tools-test)
include_directories(${PROJECT_SOURCE_DIR}/sample-net-test)
include_directories(${PROJECT_SOURCE_DIR}/sample-db-test)
include_directories(${PROJECT_SOURCE_DIR}/sample-datacenter-test)
add_subdirectory(sample-tools-test)
add_subdirectory(sample-net-test)
add_subdirectory(sample-db-test)
add_subdirectory(sample-datacenter-test)

message("[start-cpp-vcpkg] -------- done")
