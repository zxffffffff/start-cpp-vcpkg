message("[sample-tools-test] -------- start")
project ("sample-tools-test")

message(STATUS "[sample-tools-test] PROJECT_SOURCE_DIR : ${PROJECT_SOURCE_DIR}")

# 引入 GTest
enable_testing()
find_package(GTest CONFIG REQUIRED)

# 将源代码添加到此项目的可执行文件。
add_executable (${PROJECT_NAME}
    "SampleToolsTest.cpp"
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
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION bin
    ARCHIVE DESTINATION lib
)

# 运行 GTest
include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})

message("[sample-tools-test] -------- done")