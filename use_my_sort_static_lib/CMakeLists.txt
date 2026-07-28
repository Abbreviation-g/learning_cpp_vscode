cmake_minimum_required(VERSION 3.16)
project(use_my_sort_static_lib VERSION 1.0.0 LANGUAGES CXX)

# 使用固定的 C++17 标准，确保消费端与示例静态库的语言配置一致。
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(use_my_sort_static_lib src/main.cpp)

# 通过 CMAKE_PREFIX_PATH 查找已安装的库（路径在 CMakePresets.json 中配置）
find_package(my_sort_static_lib REQUIRED)

# 链接导入的静态库目标
target_link_libraries(use_my_sort_static_lib PRIVATE
    my_sort_static_lib::my_sort_static_lib
)
