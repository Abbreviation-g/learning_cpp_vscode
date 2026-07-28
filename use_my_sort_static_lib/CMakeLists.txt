cmake_minimum_required(VERSION 3.16)
project(use_my_sort_static_lib VERSION 1.0.0 LANGUAGES CXX)

# 使用固定的 C++17 标准，确保消费端与示例静态库的语言配置一致。
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(use_my_sort_static_lib src/main.cpp)

# CMAKE_PREFIX_PATH 已在 CMakePresets.json 中设置为 C:\install\my_sort_static_lib
# 添加头文件搜索路径
target_include_directories(use_my_sort_static_lib PRIVATE
    "${CMAKE_PREFIX_PATH}/include"
)

# 添加库文件搜索路径
target_link_directories(use_my_sort_static_lib PRIVATE
    "${CMAKE_PREFIX_PATH}/lib"
)

# 链接静态库（CMake 会在上面的 link directories 中查找 libmy_sort_static_lib.a）
target_link_libraries(use_my_sort_static_lib PRIVATE
    my_sort_static_lib
)
