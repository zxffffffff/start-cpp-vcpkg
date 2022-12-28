message("[sample-datacenter] -------- start")
project(sample-datacenter)

message(STATUS "[sample-datacenter] PROJECT_SOURCE_DIR : ${PROJECT_SOURCE_DIR}")

# dllµ¼³öºê
add_compile_definitions(SAMPLE_DATACENTER_EXPORTS)

add_library(${PROJECT_NAME} SHARED
    SampleDataCenter.cpp
    SampleDataCenter.h
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC sample-tools
    PRIVATE sample-pb
    PRIVATE sample-net
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

message("[sample-datacenter] -------- done")