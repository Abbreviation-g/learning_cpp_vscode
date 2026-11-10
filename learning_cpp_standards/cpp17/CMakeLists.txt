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

# c++17 文件系统 - 文件路径以及文件的状态和属性
add_executable(cpp17_filesystem1 cpp17_filesystem1/main.cpp)

# c++17 文件系统 - 文件与目录的 CRUD
add_executable(cpp17_filesystem cpp17_filesystem/main.cpp)

# c++17 搜索器
add_executable(cpp17_search cpp17_search/main.cpp)

# c++17 多线程之读写锁
add_executable(cpp17_shared_mutex cpp17_shared_mutex/main.cpp)

# c++17 智能指针的增强
add_executable(cpp17_smart_pointer cpp17_smart_pointer/main.cpp)

# c++17 并行算法
add_executable(cpp17_parallel_algorithms cpp17_parallel_algorithms/main.cpp)

# c++17 将左值引用转换为常量引用 - std::as_const
add_executable(cpp17_as_const cpp17_as_const/main.cpp)

# c++17 字符串视图（非拥有视图）- string_view
add_executable(cpp17_string_view cpp17_string_view/main.cpp)

# c++17 薛定谔的盒子 - std::optional
add_executable(cpp17_optional cpp17_optional/main.cpp)

# c++17 类型安全的联合体 -- std::variant
add_executable(cpp17_variant cpp17_variant/main.cpp)

# c++17 类型安全的任意类型容器 - std::any
add_executable(cpp17_any cpp17_any/main.cpp)

# c++17 std::map/set/unordered_map/unordered_set 的改进
add_executable(cpp17_map_set cpp17_map_set/main.cpp)

# c++17 通用否定器 - std::not_fn
add_executable(cpp17_not_fn cpp17_not_fn/main.cpp)

# c++17 终极元组解包器 - std::apply / std::make_from_tuple
add_executable(cpp17_apply cpp17_apply/main.cpp)

# c++17 万能调用神技 std::invoke
add_executable(cpp17_invoke cpp17_invoke/main.cpp)

# c++17 类模板参数推导
add_executable(cpp17_ctad cpp17_ctad/main.cpp)

# c++17 带有 auto 类型的非类型模板参数
add_executable(cpp17_template_auto cpp17_template_auto/main.cpp)

# c++17 折叠表达式
add_executable(cpp17_fold_expressions cpp17_fold_expressions/main.cpp)

# c++17 枚举的直接列表初始化 - 对 enum class 的改进
add_executable(cpp17_enum_init cpp17_enum_init/main.cpp)

# c++17 带有基类的聚合类
add_executable(cpp17_aggregate cpp17_aggregate/main.cpp)

# c++17 属性增强
add_executable(cpp17_attributes cpp17_attributes/main.cpp)
# 为 cpp17_attributes 添加编译选项 -Wno-unused-result，仅对 GNU 和 Clang 编译器生效，不影响其他 target。
# target_compile_options(cpp17_attributes PRIVATE
#     $<$<CXX_COMPILER_ID:GNU,Clang>:-Wno-unused-result>
# )

# c++17 嵌套命名空间
add_executable(cpp17_namespace cpp17_namespace/main.cpp)

# c++17 UTF-8字面量和十六进制浮点数字面量
add_executable(cpp17_u8_0x_literal cpp17_u8_0x_literal/main.cpp)

# c++17 类型萃取
add_executable(cpp17_type_traits cpp17_type_traits/main.cpp)

# c++17 constexpr if
add_executable(cpp17_constexpr_if cpp17_constexpr_if/main.cpp)

# c++17 内联变量
add_executable(cpp17_inline_variables cpp17_inline_variables/main.cpp cpp17_inline_variables/constants.cpp)