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
message(STATUS "[${PROJECT_NAME}] Protobuf_PROTOC_EXECUTABLE: ${Protobuf_PROTOC_EXECUTABLE}")

# 由于protobuf_generate_cpp只能编译一个目录下的proto，需要手动执行protoc命令
file(REAL_PATH pb proto_path)
file(REAL_PATH pb_cpp cpp_out)
message(STATUS "[${PROJECT_NAME}] proto_path: ${proto_path}")
message(STATUS "[${PROJECT_NAME}] cpp_out: ${cpp_out}")
file(MAKE_DIRECTORY ${cpp_out})
file(GLOB_RECURSE proto_files ${proto_path}/*.proto)
foreach(proto_file ${proto_files})
    execute_process(
        COMMAND ${Protobuf_PROTOC_EXECUTABLE}
            --proto_path=${Protobuf_INCLUDE_DIRS}
            --proto_path=${proto_path}
            --cpp_out=${cpp_out}
            ${proto_file}
        RESULT_VARIABLE result
        OUTPUT_VARIABLE output
        ERROR_VARIABLE error
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_STRIP_TRAILING_WHITESPACE
    )
    if(result EQUAL 0)
        message(STATUS "[${PROJECT_NAME}] Successfully processed ${proto_file}")
    else()
        message(FATAL_ERROR "[${PROJECT_NAME}] Failed to process ${proto_file}: ${error}")
    endif()
endforeach()

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
    RUNTIME DESTINATION bin # .dll
    LIBRARY DESTINATION bin # .dylib .so
    ARCHIVE DESTINATION lib # .lib .a
)

message("[${PROJECT_NAME}] -------- done")
