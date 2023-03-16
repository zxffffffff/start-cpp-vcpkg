#############################################################################
## MIT License
## 
## Author	: xiaofeng.zhu
## Support	: zxffffffff@outlook.com, 1337328542@qq.com
## 
#############################################################################

project(sample-dynamic-lib)
message("[${PROJECT_NAME}] -------- start")

message(STATUS "[${PROJECT_NAME}] PROJECT_SOURCE_DIR : ${PROJECT_SOURCE_DIR}")

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

message("[${PROJECT_NAME}] -------- done")