cmake_minimum_required(VERSION 3.16)
project(cmake-demo VERSION 1.0.0 LANGUAGES CXX)

# C++ 标准设置
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 可执行文件
add_executable(${PROJECT_NAME})
add_subdirectory(src)

# 主程序直接依赖消息/排序工具库和链表工具库。
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        message_utils
        sort_utils
)
