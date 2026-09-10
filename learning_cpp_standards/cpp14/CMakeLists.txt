cmake_minimum_required(VERSION 3.16)
project(cpp14 VERSION 1.0.0 LANGUAGES CXX)

# https://en.cppreference.com/cpp/14
# ============================================================================
# 独立的 C++14 工程
#
# 本工程只演示 C++14 特性, 编译标准固定为 C++14。
# 每个 <feature>/main.cpp 都是一个独立的可执行 target。
#
# 注意: 个别演示文件用到了 C++17/C++20 才有的库设施 (例如 std::format),
#       这类 target 通过 target_compile_features(... cxx_std_20) 单独声明
#       其真实所需标准, 工程默认标准仍然是 C++14。
#
# 构建 (在本目录下):
#   cmake --preset mingw
#   cmake --build --preset mingwbuild
# 产物: build/mingw/bin/<target>[.exe]
# ============================================================================

# C++ 标准设置
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 输出目录设置
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 查找线程库(MinGW需要)
find_package(Threads REQUIRED)

# c++14 C++14 中聚合类成员初始化 (使用了 std::format => 实际需要 C++20)
add_executable(cpp14_aggregate_class cpp14_aggregate_class/main.cpp)
target_compile_features(cpp14_aggregate_class PRIVATE cxx_std_20)

# c++14 auto 和 decltype 的改进
add_executable(cpp14_auto_decltype cpp14_auto_decltype/main.cpp)

# c++14 二进制字面量
add_executable(cpp14_binary_literal cpp14_binary_literal/main.cpp)

# c++14 constexpr限制放宽
add_executable(cpp14_constexpr cpp14_constexpr/main.cpp)

# c++14 自定义字面量
add_executable(cpp14_custom_literal cpp14_custom_literal/main.cpp)

# c++14 泛型 Lambda 以及捕获变量的增强
add_executable(cpp14_lambda cpp14_lambda/main.cpp)

# c++14 多线程之读写锁 (使用了 std::format => 实际需要 C++20)
add_executable(cpp14_shared_lock cpp14_shared_lock/main.cpp)
target_compile_features(cpp14_shared_lock PRIVATE cxx_std_20)
target_link_libraries(cpp14_shared_lock Threads::Threads)

# c++14 透明操作符函数对象
add_executable(cpp14_transparent_comparator cpp14_transparent_comparator/main.cpp)

# c++14 unique_ptr 在 C++14 中的优化
add_executable(cpp14_unique_ptr cpp14_unique_ptr/main.cpp)

# c++14 C++14 变量模板
add_executable(cpp14_variable_templates cpp14_variable_templates/main.cpp)
