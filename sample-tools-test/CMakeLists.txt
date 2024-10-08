#############################################################################
## MIT License
## 
## Author	: xiaofeng.zhu
## Support	: zxffffffff@outlook.com, 1337328542@qq.com
## 
#############################################################################

project ("sample-tools-test")
message("[${PROJECT_NAME}] -------- start")

message(STATUS "[${PROJECT_NAME}] PROJECT_SOURCE_DIR : ${PROJECT_SOURCE_DIR}")

# 引入 GTest
enable_testing()
find_package(GTest CONFIG REQUIRED)

add_executable (${PROJECT_NAME}
    log_test.cpp
    benchmark_test.cpp
    crypto_test.cpp
    snappy_test.cpp
    zlib_test.cpp
    json_test.cpp
    threadpool_test.cpp
    threadtimer_test.cpp
    fmt_test.cpp
    common_test.cpp
    platform_test.cpp
    hardware_test.cpp
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE sample-tools
    PRIVATE GTest::gtest_main
)

target_include_directories(${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}
)

install(TARGETS ${PROJECT_NAME}
    PUBLIC_HEADER DESTINATION include
    RUNTIME DESTINATION bin # .dll
    LIBRARY DESTINATION bin # .dylib .so
    ARCHIVE DESTINATION lib # .lib .a
)

# 运行 GTest
include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})

message("[${PROJECT_NAME}] -------- done")