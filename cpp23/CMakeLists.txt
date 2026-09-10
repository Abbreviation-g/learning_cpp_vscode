cmake_minimum_required(VERSION 3.16)
project(cpp23 VERSION 1.0.0 LANGUAGES CXX)
# https://en.cppreference.com/cpp/23
# C++ 标准设置
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 输出目录设置
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 查找线程库(MinGW需要)
find_package(Threads REQUIRED)

# 可执行文件
add_executable(${PROJECT_NAME}
    src/main.cpp
)

# C++17 lambda 特性演示 target
add_executable(cpp17_lambda
    cpp17_lambda/main.cpp
)
target_link_libraries(cpp17_lambda Threads::Threads)

# C++17 if/switch 特性演示 target
add_executable(cpp17_if_switch
    cpp17_if_switch/main.cpp
)

# C++17 结构化绑定 特性演示 target
add_executable(cpp17_structured_binding
    cpp17_structured_binding/main.cpp
)

# C++17 std::byte 特性演示 target
add_executable(cpp17_byte
    cpp17_byte/main.cpp
)

# C++17 std::charconv 特性演示 target
add_executable(cpp17_charconv
    cpp17_charconv/main.cpp
)

# C++20中头文件span的使用
add_executable(cpp20_span cpp20_span/main.cpp)

# C++20中头文件syncstream的使用
add_executable(cpp20_syncstream cpp20_syncstream/main.cpp)

# C++20中线程类std::jthread的使用
add_executable(cpp20_jthread cpp20_jthread/main.cpp)

# C++20中头文件source_location的使用
add_executable(cpp20_source_location cpp20_source_location/main.cpp)

# C++20中头文件source_location的使用
add_executable(cpp20_ranges cpp20_ranges/main.cpp)

# ============================================================================
# C++23 标准库模块 (import std;)
#
# 依赖 libstdc++ 的 std 模块 (gcm.cache/std.gcm), 该模块通过 VS Code task
# "GCC: build std module (mingw)" 在 build/mingw 下构建:
#   g++ -std=c++23 -fmodules-ts -fsearch-include-path -c bits/std.cc
#
# 本 target 只需开启 -fmodules 编译单元的模块支持。CMake 编译时工作目录为
# ${CMAKE_CURRENT_BINARY_DIR} (= build/<preset>), 而 std 模块缓存
# (gcm.cache/std.gcm 及 bits/stdc++.h.gcm) 也位于该目录, 默认模块映射
# 即可找到, 无需额外 mapper。
# ============================================================================
# add_executable(cpp23_import cpp23_import/main.cpp)
# if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
#     # GNU 工具链: 启用模块编译支持
#     target_compile_options(cpp23_import PRIVATE -fmodules-ts)
# elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
#     # Clang: 模块支持需显式启用 (若使用 libc++ 的 std 模块场景)
#     target_compile_options(cpp23_import PRIVATE -fmodules)
# endif()

# c++14 二进制字面量
add_executable(cpp14_binary_literal cpp14_binary_literal/main.cpp)

# c++11 自定义字面量
add_executable(cpp11_custom_literal cpp11_custom_literal/main.cpp)

# c++14 自定义字面量
add_executable(cpp14_custom_literal cpp14_custom_literal/main.cpp)

# c++14 constexpr限制放宽
add_executable(cpp14_constexpr cpp14_constexpr/main.cpp)

# c++14 泛型 Lambda 以及捕获变量的增强
add_executable(cpp14_lambda cpp14_lambda/main.cpp)

# c++14 auto 和 decltype 的改进
add_executable(cpp14_auto_decltype cpp14_auto_decltype/main.cpp)

# c++14 C++14 中聚合类成员初始化
add_executable(cpp14_aggregate_class cpp14_aggregate_class/main.cpp)

# c++14 C++14 变量模板
add_executable(cpp14_variable_templates cpp14_variable_templates/main.cpp)

# c++14 透明操作符函数对象
add_executable(cpp14_transparent_comparator cpp14_transparent_comparator/main.cpp)

# c++14 unique_ptr 在 C++14 中的优化
add_executable(cpp14_unique_ptr cpp14_unique_ptr/main.cpp)

# c++14 多线程之读写锁
add_executable(cpp14_shared_lock cpp14_shared_lock/main.cpp)
target_link_libraries(cpp14_shared_lock Threads::Threads)

# c++11 原始字面量
add_executable(cpp11_r_string cpp11_r_string/main.cpp)

# c++11 指针空值类型 - nullptr
add_executable(cpp11_nullptr cpp11_nullptr/main.cpp)

# c++11 constexpr
add_executable(cpp11_constexpr cpp11_constexpr/main.cpp)