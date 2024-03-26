#############################################################################
## MIT License
## 
## Author	: xiaofeng.zhu
## Support	: zxffffffff@outlook.com, 1337328542@qq.com
## 
#############################################################################

project ("sample-main")
message("[${PROJECT_NAME}] -------- start")

message(STATUS "[${PROJECT_NAME}] PROJECT_SOURCE_DIR : ${PROJECT_SOURCE_DIR}")

add_executable(${PROJECT_NAME}
    "main.cpp"
)

if (MSVC)
# 手动启用 C++ 异常: https://learn.microsoft.com/zh-cn/cpp/build/reference/eh-exception-handling-model
set_target_properties(${PROJECT_NAME}
    PROPERTIES
    COMPILE_FLAGS "/EHsc"
)
endif()

target_link_libraries(${PROJECT_NAME}
    PUBLIC sample-dynamic-lib
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

message("[${PROJECT_NAME}] -------- done")