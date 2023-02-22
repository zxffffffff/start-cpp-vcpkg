#############################################################################
## MIT License
## 
## Author	: xiaofeng.zhu
## Support	: zxffffffff@outlook.com, 1337328542@qq.com
## 
#############################################################################

message("[sample-db-test] -------- start")
project ("sample-db-test")

message(STATUS "[sample-db-test] PROJECT_SOURCE_DIR : ${PROJECT_SOURCE_DIR}")

# 引入 GTest
enable_testing()
find_package(GTest CONFIG REQUIRED)

# 将源代码添加到此项目的可执行文件。
add_executable (${PROJECT_NAME}
    SampleDBTest.cpp
    "mysql/MySQLClientTest.cpp"
    "redis/RedisClientTest.cpp"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE sample-db
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

message("[sample-db-test] -------- done")