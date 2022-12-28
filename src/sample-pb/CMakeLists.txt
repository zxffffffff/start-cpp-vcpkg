message("[sample-pb] -------- start")
project(sample-pb)

message(STATUS "[sample-pb] PROJECT_SOURCE_DIR : ${PROJECT_SOURCE_DIR}")

# 引入protobuf库
find_package(protobuf CONFIG REQUIRED)
message(STATUS "[sample-pb] Protobuf_VERSION : ${Protobuf_VERSION}")
message(STATUS "[sample-pb] Protobuf_INCLUDE_DIRS : ${Protobuf_INCLUDE_DIRS}")
message(STATUS "[sample-pb] Protobuf_LIBRARIES : ${Protobuf_LIBRARIES}")

# ::由于PROTOBUF_GENERATE_CPP只能编译一个目录下的proto，尽量选择手动生成pb.cc，参考general-cpp.bat

set(sample_files
    pb_cpp/MsgType.pb.cc
    pb_cpp/Req.pb.cc
    pb_cpp/Res.pb.cc
    pb_cpp/common/Stock.pb.cc
    pb_cpp/notify/QuotePushTickNotify.pb.cc
    pb_cpp/req/HeartbeatReq.pb.cc
    pb_cpp/req/QuoteSubscribeTickReq.pb.cc
    pb_cpp/req/QuoteUnsubscribeTickReq.pb.cc
    pb_cpp/res/HeartbeatRes.pb.cc
    pb_cpp/res/QuoteSubscribeTickRes.pb.cc
    pb_cpp/res/QuoteUnsubscribeTickRes.pb.cc
    SamplePB.cpp
)

set(sample_headers
    pb_cpp/MsgType.pb.cc
    pb_cpp/Req.pb.cc
    pb_cpp/Res.pb.cc
    pb_cpp/common/Stock.pb.cc
    pb_cpp/notify/QuotePushTickNotify.pb.cc
    pb_cpp/req/HeartbeatReq.pb.cc
    pb_cpp/req/QuoteSubscribeTickReq.pb.cc
    pb_cpp/req/QuoteUnsubscribeTickReq.pb.cc
    pb_cpp/res/HeartbeatRes.pb.cc
    pb_cpp/res/QuoteSubscribeTickRes.pb.cc
    pb_cpp/res/QuoteUnsubscribeTickRes.pb.cc
    SamplePB.h
)

add_library(${PROJECT_NAME} STATIC
    ${sample_files}
    ${sample_headers}
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC protobuf::libprotoc protobuf::libprotobuf protobuf::libprotobuf-lite
)

# 不暴露protobuf相关内容，仅暴露SmaplePB
target_include_directories(${PROJECT_NAME}
    PUBLIC ${Protobuf_INCLUDE_DIRS}
    PUBLIC ${PROJECT_SOURCE_DIR}/pb_cpp
    PUBLIC ${PROJECT_SOURCE_DIR}
)

install(TARGETS ${PROJECT_NAME}
    PUBLIC_HEADER DESTINATION include
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION bin
    ARCHIVE DESTINATION lib
)

message("[sample-pb] -------- done")