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
message(STATUS "[${PROJECT_NAME}] CURL_LIBRARIES : ${CURL_LIBRARIES}")

# 引入 boost-url (需要link)
find_package(Boost REQUIRED COMPONENTS url)

# Public 头文件
file(GLOB_RECURSE include_headers
    net_common.h
    net_states.h
    tcp_client.h
    tcp_server.h
    http_server.h
    http_client.h
    interface/tcp_interface.h
    interface/http_interface.h
    impl/libuv_event_loop.h
    impl/libuv_tcp_client_impl.h
    impl/libuv_tcp_server_impl.h
    impl/boost_http_parser.h
    impl/curl_http_client.h
)

# 至少要有一个cpp，否则部分平台无法编译通过
add_library(${PROJECT_NAME} STATIC
    sample/sample_net.cpp
    ${include_headers}
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC sample-tools
    PRIVATE $<IF:$<TARGET_EXISTS:libuv::uv_a>,libuv::uv_a,libuv::uv>
    PRIVATE CURL::libcurl
    PRIVATE Boost::url
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
