#############################################################################
## MIT License
## 
## Author	: xiaofeng.zhu
## Support	: zxffffffff@outlook.com, 1337328542@qq.com
## 
#############################################################################

message("[sample-db] -------- start")
project(sample-db)

message(STATUS "[sample-db] PROJECT_SOURCE_DIR : ${PROJECT_SOURCE_DIR}")

# MySQL
find_package(unofficial-mysql-connector-cpp CONFIG REQUIRED)

# Redis
find_package(hiredis CONFIG REQUIRED)

# MongoDB
find_package(bsoncxx CONFIG REQUIRED)
find_package(mongocxx CONFIG REQUIRED)

add_library(${PROJECT_NAME} STATIC
    SampleDB.cpp
    SampleDB.h
    "mysql/MySQLClient.h"
    "mysql/MySQLClient.cpp"
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC sample-tools
    PRIVATE unofficial::mysql-connector-cpp::connector
    PRIVATE hiredis::hiredis
    PRIVATE mongo::bsoncxx_shared
    PRIVATE mongo::mongocxx_shared
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

message("[sample-db] -------- done")