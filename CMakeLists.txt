message("[cpp-deps-vcpkg] -------- start")
cmake_minimum_required (VERSION 3.16)
set(CMAKE_CXX_STANDARD 20)

if (WIN32)
    # ...
else()
    # linux 需要设置-fPIC
    set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

# 使用 vcpkg 工具链，需要在 project 之前设置
set(CMAKE_TOOLCHAIN_FILE "./vcpkg/scripts/buildsystems/vcpkg.cmake")
message(STATUS "[cpp-deps-vcpkg] CMAKE_TOOLCHAIN_FILE : ${CMAKE_TOOLCHAIN_FILE}")

project ("cpp-deps-vcpkg")

message(STATUS "[cpp-deps-vcpkg] PROJECT_SOURCE_DIR : ${PROJECT_SOURCE_DIR}")

include_directories(${PROJECT_SOURCE_DIR}/src/sample-tools)
include_directories(${PROJECT_SOURCE_DIR}/src/sample-pb)
include_directories(${PROJECT_SOURCE_DIR}/src/sample-net)
include_directories(${PROJECT_SOURCE_DIR}/src/sample-datacenter)
include_directories(${PROJECT_SOURCE_DIR}/src/sample-main)
add_subdirectory(src/sample-tools)
add_subdirectory(src/sample-pb)
add_subdirectory(src/sample-net)
add_subdirectory(src/sample-datacenter)
add_subdirectory(src/sample-main)

include_directories(${PROJECT_SOURCE_DIR}/test/sample-tools-test)
include_directories(${PROJECT_SOURCE_DIR}/test/sample-net-test)
include_directories(${PROJECT_SOURCE_DIR}/test/sample-datacenter-test)
add_subdirectory(test/sample-tools-test)
add_subdirectory(test/sample-net-test)
add_subdirectory(test/sample-datacenter-test)

message("[cpp-deps-vcpkg] -------- done")