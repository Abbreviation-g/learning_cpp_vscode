cmake_minimum_required(VERSION 3.16)
project(cpp11 VERSION 1.0.0 LANGUAGES CXX)

# https://en.cppreference.com/cpp/11
# ============================================================================
# 独立的 C++11 工程
#
# 本工程只演示 C++11 特性, 编译标准固定为 C++11。
# 每个 <feature>/main.cpp 都是一个独立的可执行 target。
#
# 构建 (在本目录下):
#   cmake --preset mingw
#   cmake --build --preset mingwbuild
# 产物: build/mingw/bin/<target>[.exe]
# ============================================================================

# C++ 标准设置
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 输出目录设置
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 查找线程库(MinGW需要)
find_package(Threads REQUIRED)

# c++11 constexpr
add_executable(cpp11_constexpr cpp11_constexpr/main.cpp)

# c++11 自定义字面量
add_executable(cpp11_custom_literal cpp11_custom_literal/main.cpp)

# c++11 指针空值类型 - nullptr
add_executable(cpp11_nullptr cpp11_nullptr/main.cpp)

# c++11 原始字面量
add_executable(cpp11_r_string cpp11_r_string/main.cpp)

# c++11 auto decltype 自动类型推导
add_executable(cpp11_auto_decltype cpp11_auto_decltype/main.cpp)

# c++11 final和override
add_executable(cpp11_final_override cpp11_final_override/main.cpp)

# c++11 模板的优化
add_executable(cpp11_template cpp11_template/main.cpp)

# c++11 using的使用
add_executable(cpp11_using cpp11_using/main.cpp)

# c++11 委托构造和继承构造函数
add_executable(cpp11_constructor cpp11_constructor/main.cpp)

# c++11 列表初始化
add_executable(cpp11_list_init cpp11_list_init/main.cpp)

# c++11 基于范围的for循环
add_executable(cpp11_for_range cpp11_for_range/main.cpp)