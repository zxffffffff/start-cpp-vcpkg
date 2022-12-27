# The real minimum version will be checked by the qtbase project.
# 3.16 is the absolute minimum though.
cmake_minimum_required (VERSION 3.16)

# 使用 vcpkg 工具链，需要在 project 之前设置
set(CMAKE_TOOLCHAIN_FILE "./vcpkg/scripts/buildsystems/vcpkg.cmake")
message(STATUS "[cpp-deps-vcpkg] CMAKE_TOOLCHAIN_FILE : ${CMAKE_TOOLCHAIN_FILE}")

project ("cpp-deps-vcpkg")
message("[cpp-deps-vcpkg] -------- start")

message(STATUS "[cpp-deps-vcpkg] PROJECT_SOURCE_DIR : ${PROJECT_SOURCE_DIR}")

# include搜索目录
include_directories(${PROJECT_SOURCE_DIR}/src/sample-pb)
include_directories(${PROJECT_SOURCE_DIR}/src/sample-tools)
include_directories(${PROJECT_SOURCE_DIR}/src/sample-datacenter)
include_directories(${PROJECT_SOURCE_DIR}/src/sample-main)

# 将子目录添加到生成
add_subdirectory(src/sample-pb)
add_subdirectory(src/sample-tools)
add_subdirectory(src/sample-datacenter)
add_subdirectory(src/sample-main)

message("[cpp-deps-vcpkg] -------- done")