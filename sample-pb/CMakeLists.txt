#############################################################################
## MIT License
## 
## Author	: xiaofeng.zhu
## Support	: zxffffffff@outlook.com, 1337328542@qq.com
## 
#############################################################################

project(sample-pb)
message("[${PROJECT_NAME}] -------- start")

message(STATUS "[${PROJECT_NAME}] PROJECT_SOURCE_DIR : ${PROJECT_SOURCE_DIR}")

# 引入protobuf库
find_package(Protobuf CONFIG REQUIRED)
message(STATUS "[${PROJECT_NAME}] Protobuf_VERSION : ${Protobuf_VERSION}")
message(STATUS "[${PROJECT_NAME}] Protobuf_INCLUDE_DIRS : ${Protobuf_INCLUDE_DIRS}")
message(STATUS "[${PROJECT_NAME}] Protobuf_LIBRARIES : ${Protobuf_LIBRARIES}")

# 由于PROTOBUF_GENERATE_CPP只能编译一个目录下的proto，尽量选择手动生成pb.cc，参考general-cpp.bat

# Public 头文件
file(GLOB_RECURSE c_headers pb_cpp/*.h)
file(GLOB_RECURSE c_files pb_cpp/*.cc)

add_library(${PROJECT_NAME} STATIC
    SamplePB.cpp
    ${c_files}
    ${c_headers}
)

# protobuf::libprotoc         编译器.proto生成code
# protobuf::libprotobuf       使用Messager序列号和反序列化
# protobuf::libprotobuf-lite  使用MessageLite代替Messager，常用于移动端
target_link_libraries(${PROJECT_NAME}
    PUBLIC protobuf::libprotobuf
)

target_include_directories(${PROJECT_NAME}
    PUBLIC ${Protobuf_INCLUDE_DIRS}
    PUBLIC ${PROJECT_SOURCE_DIR}/pb_cpp
    PUBLIC ${PROJECT_SOURCE_DIR}
)

target_include_directories(${PROJECT_NAME}
    PUBLIC ${c_headers}
)

install(TARGETS ${PROJECT_NAME}
    PUBLIC_HEADER DESTINATION include
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION bin
    ARCHIVE DESTINATION lib
)

message("[${PROJECT_NAME}] -------- done")