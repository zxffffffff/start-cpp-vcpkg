# The real minimum version will be checked by the qtbase project.
# 3.16 is the absolute minimum though.
cmake_minimum_required (VERSION 3.16)

project(sample-datacenter)
message("[sample-datacenter] -------- start")

message(STATUS "[sample-datacenter] PROJECT_SOURCE_DIR : ${PROJECT_SOURCE_DIR}")

# dllµ¼³öºê
add_compile_definitions(SAMPLE_DATACENTER_EXPORTS)

add_library(${PROJECT_NAME} SHARED
    SampleDataCenter.cpp
    SampleDataCenter.h
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE sample-pb
    PRIVATE sample-tools
)

target_include_directories(${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}
)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

message("[sample-datacenter] -------- done")