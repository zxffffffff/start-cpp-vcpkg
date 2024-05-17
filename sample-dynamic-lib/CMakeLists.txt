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

# Public 头文件
file(GLOB_RECURSE include_headers
    define.h
    SampleDynamicLib.h
)

add_library(${PROJECT_NAME} SHARED
    SampleDynamicLib.cpp
    ${include_headers}
)

# dynamic 导出宏
target_compile_definitions(${PROJECT_NAME}
    PRIVATE SAMPLE_DYNAMIC_LIB_EXPORT
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC sample-datacenter
)

target_include_directories(${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    PUBLIC_HEADER "${include_headers}"
)

install(TARGETS ${PROJECT_NAME}
    PUBLIC_HEADER DESTINATION include
    RUNTIME DESTINATION bin # .dll
    LIBRARY DESTINATION bin # .dylib .so
    ARCHIVE DESTINATION lib # .lib .a
)

message("[${PROJECT_NAME}] -------- done")