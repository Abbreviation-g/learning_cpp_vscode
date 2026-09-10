cmake_minimum_required(VERSION 3.16)
project(cpp17 VERSION 1.0.0 LANGUAGES CXX)

# https://en.cppreference.com/cpp/17
# ============================================================================
# 独立的 C++17 工程
#
# 本工程只演示 C++17 特性, 编译标准固定为 C++17。
# 每个 <feature>/main.cpp 都是一个独立的可执行 target。
#
# 构建 (在本目录下):
#   cmake --preset mingw
#   cmake --build --preset mingwbuild
# 产物: build/mingw/bin/<target>[.exe]
# ============================================================================

# C++ 标准设置
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 输出目录设置
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 查找线程库(MinGW需要)
find_package(Threads REQUIRED)

# C++17 lambda 特性演示 target
add_executable(cpp17_lambda cpp17_lambda/main.cpp)
target_link_libraries(cpp17_lambda Threads::Threads)

# C++17 if/switch 特性演示 target
add_executable(cpp17_if_switch cpp17_if_switch/main.cpp)

# C++17 结构化绑定 特性演示 target
add_executable(cpp17_structured_binding cpp17_structured_binding/main.cpp)

# C++17 std::byte 特性演示 target
add_executable(cpp17_byte cpp17_byte/main.cpp)

# C++17 std::charconv 特性演示 target
add_executable(cpp17_charconv cpp17_charconv/main.cpp)
