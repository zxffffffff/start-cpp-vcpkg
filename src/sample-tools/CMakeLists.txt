# The real minimum version will be checked by the qtbase project.
# 3.16 is the absolute minimum though.
cmake_minimum_required (VERSION 3.16)

project(sample-tools)
message("[sample-tools] -------- start")

message(STATUS "[sample-tools] PROJECT_SOURCE_DIR : ${PROJECT_SOURCE_DIR}")

# dll导出宏
add_compile_definitions(SAMPLE_TOOLS_EXPORTS)

# 引入zlib库
find_package(ZLIB REQUIRED)
message(STATUS "[sample-tools] ZLIB_VERSION_STRING : ${ZLIB_VERSION_STRING}")
message(STATUS "[sample-tools] ZLIB_INCLUDE_DIRS : ${ZLIB_INCLUDE_DIRS}")
message(STATUS "[sample-tools] ZLIB_LIBRARIES : ${ZLIB_LIBRARIES}")

# 引入 gflags
find_package(gflags CONFIG REQUIRED)

# 引入 glog
find_package(glog CONFIG REQUIRED)

add_library(${PROJECT_NAME} SHARED
    SampleTools.cpp
    SampleTools.h
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC ZLIB::ZLIB
    PUBLIC gflags::gflags
    PUBLIC glog::glog
)

target_include_directories(${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}
)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

message("[sample-tools] -------- done")