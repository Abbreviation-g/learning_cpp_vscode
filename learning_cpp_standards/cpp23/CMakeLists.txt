cmake_minimum_required(VERSION 3.16)
project(cpp23 VERSION 1.0.0 LANGUAGES CXX)

# https://en.cppreference.com/cpp/23
# ============================================================================
# 独立的 C++23 工程
#
# 本工程只演示 C++23 特性, 编译标准固定为 C++23。
# 每个 <feature>/main.cpp 都是一个独立的可执行 target。
#
# 构建 (在本目录下):
#   cmake --preset mingw
#   cmake --build --preset mingwbuild
# 产物: build/mingw/bin/<target>[.exe]
# ============================================================================

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

# ----------------------------------------------------------------------------
# src/main.cpp: 构造函数/赋值运算符演示 (原根工程的可执行文件, 此处独立成 target)
# ----------------------------------------------------------------------------
add_executable(cpp23_common cpp23_common/main.cpp)

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
add_executable(cpp23_import cpp23_import/main.cpp)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # GNU 工具链: 启用模块编译支持
    target_compile_options(cpp23_import PRIVATE -fmodules-ts)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # Clang: 模块支持需显式启用 (若使用 libc++ 的 std 模块场景)
    target_compile_options(cpp23_import PRIVATE -fmodules)
endif()
