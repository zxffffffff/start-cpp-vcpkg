message("[sample-tools] -------- start")
project(sample-tools)

message(STATUS "[sample-tools] PROJECT_SOURCE_DIR : ${PROJECT_SOURCE_DIR}")

# 引入zlib库
find_package(ZLIB REQUIRED)
message(STATUS "[sample-tools] ZLIB_VERSION_STRING : ${ZLIB_VERSION_STRING}")
message(STATUS "[sample-tools] ZLIB_INCLUDE_DIRS : ${ZLIB_INCLUDE_DIRS}")
message(STATUS "[sample-tools] ZLIB_LIBRARIES : ${ZLIB_LIBRARIES}")

# 引入 gflags
find_package(gflags CONFIG REQUIRED)

# 引入 glog
find_package(glog CONFIG REQUIRED)

# 引入 sqlite3
find_package(unofficial-sqlite3 CONFIG REQUIRED)

# 引入 cryptopp
find_package(cryptopp CONFIG REQUIRED)

# 引入 jsoncpp
find_package(jsoncpp CONFIG REQUIRED)

add_library(${PROJECT_NAME} STATIC
    SampleTools.cpp
    SampleTools.h
    "crypto/Crypto.h"
    "crypto/Crypto.cpp"
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC ZLIB::ZLIB
    PUBLIC gflags::gflags
    PUBLIC glog::glog
    PUBLIC unofficial::sqlite3::sqlite3
    PUBLIC cryptopp::cryptopp
    PUBLIC jsoncpp_lib jsoncpp_object JsonCpp::JsonCpp
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

message("[sample-tools] -------- done")