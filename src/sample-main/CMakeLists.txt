# The real minimum version will be checked by the qtbase project.
# 3.16 is the absolute minimum though.
cmake_minimum_required (VERSION 3.16)

project ("sample-main")
message("[sample-main] -------- start")

message(STATUS "[sample-main] PROJECT_SOURCE_DIR : ${PROJECT_SOURCE_DIR}")

# 将源代码添加到此项目的可执行文件。
add_executable (${PROJECT_NAME}
    "main.cpp"
    "main.h"
)

target_link_libraries(${PROJECT_NAME}
    sample-datacenter
)

target_include_directories(${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}
)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

message("[sample-main] -------- done")