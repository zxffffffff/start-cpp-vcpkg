message("[sample-dynamic-lib] -------- start")
project(sample-dynamic-lib)

message(STATUS "[sample-dynamic-lib] PROJECT_SOURCE_DIR : ${PROJECT_SOURCE_DIR}")

# dll导出宏
add_compile_definitions(SAMPLE_DYNAMIC_LIB_EXPORTS)

add_library(${PROJECT_NAME} SHARED
    define.h
    SampleDynamicLib.cpp
    SampleDynamicLib.h
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC sample-datacenter
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

message("[sample-dynamic-lib] -------- done")