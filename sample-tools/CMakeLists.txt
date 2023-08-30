#############################################################################
## MIT License
## 
## Author	: xiaofeng.zhu
## Support	: zxffffffff@outlook.com, 1337328542@qq.com
## 
#############################################################################

project(sample-tools)
message("[${PROJECT_NAME}] -------- start")

message(STATUS "[${PROJECT_NAME}] PROJECT_SOURCE_DIR : ${PROJECT_SOURCE_DIR}")

# 引入 zlib
find_package(ZLIB REQUIRED)
message(STATUS "[${PROJECT_NAME}] ZLIB_VERSION_STRING : ${ZLIB_VERSION_STRING}")
message(STATUS "[${PROJECT_NAME}] ZLIB_INCLUDE_DIRS : ${ZLIB_INCLUDE_DIRS}")
message(STATUS "[${PROJECT_NAME}] ZLIB_LIBRARIES : ${ZLIB_LIBRARIES}")

# 引入 gflags
find_package(gflags CONFIG REQUIRED)

# 引入 glog
find_package(glog CONFIG REQUIRED)

# 引入 sqlite3
find_package(unofficial-sqlite3 CONFIG REQUIRED)

# 引入 cryptopp
find_package(cryptopp CONFIG REQUIRED)

# 引入 rapidjson & nlohmann-json
find_package(RapidJSON CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# 引入 tinyxml
find_package(tinyxml2 CONFIG REQUIRED)

# 引入 fmt
find_package(fmt CONFIG REQUIRED)

# Public 头文件
file(GLOB_RECURSE include_headers
    chrono.h
    interface/crypto_interface.h
    interface/threadpool_interface.h
    interface/threadtimer_interface.h
    impl/cryptopp_impl.h
    impl/stl_threadpool_impl.h 
    impl/boost_threadpool_impl.h 
    impl/stl_threadtimer_impl.h 
)

add_library(${PROJECT_NAME} STATIC
    sample/sample_tools.cpp
    ${include_headers}
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC gflags::gflags
    PUBLIC glog::glog
    PUBLIC ZLIB::ZLIB
    PUBLIC unofficial::sqlite3::sqlite3
    PUBLIC cryptopp::cryptopp
    PUBLIC rapidjson
    PUBLIC nlohmann_json::nlohmann_json
    PUBLIC tinyxml2::tinyxml2
    PUBLIC fmt::fmt
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
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION bin
    ARCHIVE DESTINATION lib
)

message("[${PROJECT_NAME}] -------- done")