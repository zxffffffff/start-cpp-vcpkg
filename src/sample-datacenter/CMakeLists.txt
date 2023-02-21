#############################################################################
## MIT License
## 
## Author	: xiaofeng.zhu
## Support	: zxffffffff@outlook.com, 1337328542@qq.com
## 
#############################################################################

message("[sample-datacenter] -------- start")
project(sample-datacenter)

message(STATUS "[sample-datacenter] PROJECT_SOURCE_DIR : ${PROJECT_SOURCE_DIR}")

add_library(${PROJECT_NAME} STATIC
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