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

# 引入libuv库（uv）
find_package(libuv CONFIG REQUIRED)

# 引入curl库
find_package(CURL CONFIG REQUIRED)
message(STATUS "[sample-tools] CURL_VERSION_STRING : ${CURL_VERSION_STRING}")
message(STATUS "[sample-tools] CURL_INCLUDE_DIR : ${CURL_INCLUDE_DIR}")
message(STATUS "[sample-tools] CURL_LIBRARIES : ${CURL_LIBRARIES}")

add_library(${PROJECT_NAME} SHARED
    SampleTools.cpp
    SampleTools.h
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE ZLIB::ZLIB
    PRIVATE $<IF:$<TARGET_EXISTS:uv_a>,uv_a,uv>
    PRIVATE CURL::libcurl
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