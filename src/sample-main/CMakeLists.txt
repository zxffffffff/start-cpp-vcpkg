message("[sample-main] -------- start")
project ("sample-main")

message(STATUS "[sample-main] PROJECT_SOURCE_DIR : ${PROJECT_SOURCE_DIR}")

# 将源代码添加到此项目的可执行文件。
add_executable (${PROJECT_NAME}
    "main.cpp"
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC sample-dynamic-lib
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

message("[sample-main] -------- done")