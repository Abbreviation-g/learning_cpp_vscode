cmake_minimum_required(VERSION 3.16)
project(cpp20 VERSION 1.0.0 LANGUAGES CXX)

# https://en.cppreference.com/cpp/20
# ============================================================================
# 独立的 C++20 工程
#
# 本工程只演示 C++20 特性, 编译标准固定为 C++20。
# 每个 <feature>/main.cpp 都是一个独立的可执行 target。
#
# 构建 (在本目录下):
#   cmake --preset mingw
#   cmake --build --preset mingwbuild
# 产物: build/mingw/bin/<target>[.exe]
# ============================================================================

# C++ 标准设置
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 输出目录设置
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 查找线程库(MinGW需要)
find_package(Threads REQUIRED)

# C++20中线程类std::jthread的使用
add_executable(cpp20_jthread cpp20_jthread/main.cpp)
target_link_libraries(cpp20_jthread Threads::Threads)

# C++20 concepts / ranges
add_executable(cpp20_ranges cpp20_ranges/main.cpp)

# C++20中头文件source_location的使用
add_executable(cpp20_source_location cpp20_source_location/main.cpp)

# C++20中头文件span的使用
add_executable(cpp20_span cpp20_span/main.cpp)

# C++20中头文件syncstream的使用
add_executable(cpp20_syncstream cpp20_syncstream/main.cpp)
target_link_libraries(cpp20_syncstream Threads::Threads)
