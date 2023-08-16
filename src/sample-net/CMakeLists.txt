#############################################################################
## MIT License
## 
## Author	: xiaofeng.zhu
## Support	: zxffffffff@outlook.com, 1337328542@qq.com
## 
#############################################################################

project(sample-net)
message("[${PROJECT_NAME}] -------- start")

message(STATUS "[${PROJECT_NAME}] PROJECT_SOURCE_DIR : ${PROJECT_SOURCE_DIR}")

# 引入libuv库
find_package(libuv CONFIG REQUIRED)

# 引入curl库
find_package(CURL CONFIG REQUIRED)
message(STATUS "[${PROJECT_NAME}] CURL_VERSION_STRING : ${CURL_VERSION_STRING}")
message(STATUS "[${PROJECT_NAME}] CURL_INCLUDE_DIR : ${CURL_INCLUDE_DIR}")
message(STATUS "[${PROJECT_NAME}] CURL_LIBRARIES : ${CURL_LIBRARIES}")

# Public 头文件
file(GLOB_RECURSE include_headers
    SampleNet.h
    http/HttpClient.h
    tcp/tcp_proto.h
    tcp/tcp_interface.h
    tcp/tcp_package.h
    tcp/tcp_client.h
    tcp/tcp_server.h
    tcp/impl/libuv_event_loop.h
    tcp/impl/libuv_tcp_client_impl.h
    tcp/impl/libuv_tcp_server_impl.h
)

add_library(${PROJECT_NAME} STATIC
    SampleNet.cpp
    http/HttpClient.cpp
    ${include_headers}
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC sample-tools
    PRIVATE $<IF:$<TARGET_EXISTS:uv_a>,uv_a,uv>
    PRIVATE CURL::libcurl
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