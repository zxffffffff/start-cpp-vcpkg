#############################################################################
## MIT License
## 
## Author	: xiaofeng.zhu
## Support	: zxffffffff@outlook.com, 1337328542@qq.com
## 
#############################################################################

project(sample-db)
message("[${PROJECT_NAME}] -------- start")

message(STATUS "[${PROJECT_NAME}] PROJECT_SOURCE_DIR : ${PROJECT_SOURCE_DIR}")

# MySQL
# find_package(unofficial-mysql-connector-cpp CONFIG REQUIRED)

# Redis
find_package(hiredis CONFIG REQUIRED)

# MongoDB
# find_package(bsoncxx CONFIG REQUIRED)
# find_package(mongocxx CONFIG REQUIRED)

# Public 头文件
file(GLOB_RECURSE include_headers
    SampleDB.h
    # "mysql/MySQLClient.h"
    "redis/RedisCommon.h"
    "redis/RedisClient.h"
)

add_library(${PROJECT_NAME} STATIC
    SampleDB.cpp
    # "mysql/MySQLClient.cpp"
    "redis/RedisClient.cpp"
    ${include_headers}
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC sample-tools
    # PRIVATE unofficial::mysql-connector-cpp::connector
    PRIVATE hiredis::hiredis
    # PRIVATE mongo::bsoncxx_shared
    # PRIVATE mongo::mongocxx_shared
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