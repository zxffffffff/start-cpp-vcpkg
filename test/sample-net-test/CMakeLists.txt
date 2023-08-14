#############################################################################
## MIT License
## 
## Author	: xiaofeng.zhu
## Support	: zxffffffff@outlook.com, 1337328542@qq.com
## 
#############################################################################

project ("sample-net-test")
message("[${PROJECT_NAME}] -------- start")

message(STATUS "[${PROJECT_NAME}] PROJECT_SOURCE_DIR : ${PROJECT_SOURCE_DIR}")

# 引入 GTest
enable_testing()
find_package(GTest CONFIG REQUIRED)

# Public 头文件
add_executable (${PROJECT_NAME}
    SampleNetTest.cpp
    http/HttpTest.cpp
    tcp/tcp_test.cpp
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE sample-net
    PRIVATE GTest::gtest_main
)

target_include_directories(${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}
)

install(TARGETS ${PROJECT_NAME}
    PUBLIC_HEADER DESTINATION include
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION bin
    ARCHIVE DESTINATION lib
)

# 运行 GTest
include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})

message("[${PROJECT_NAME}] -------- done")