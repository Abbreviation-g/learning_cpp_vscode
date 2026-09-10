cmake_minimum_required(VERSION 3.16)
project(learning_cpp_standards VERSION 1.0.0 LANGUAGES CXX)

# ============================================================================
# 汇总工程 (aggregate)
#
# 本工程本身不定义任何 target, 只通过 add_subdirectory 把 5 个按 C++ 标准
# 拆分的独立子工程聚合在一起, 便于一次性整体构建:
#
#   cmake --preset mingw
#   cmake --build --preset mingwbuild
#
# 注意: 各子工程在自己的 CMakeLists.txt 中各自设置 CMAKE_CXX_STANDARD
# (11 / 14 / 17 / 20 / 23), 因此整体构建时每个子工程仍然使用各自的标准,
# 互不影响。
#
# 目录结构:
#   cpp11/  独立的 C++11 工程
#   cpp14/  独立的 C++14 工程
#   cpp17/  独立的 C++17 工程
#   cpp20/  独立的 C++20 工程
#   cpp23/  独立的 C++23 工程 (含 src/main.cpp 与 cpp23_import 模块演示)
#
# 每个子工程也都有自己完整的 CMakePresets.json、.clangd、.clang-format、
# .clang-tidy 与 .vscode/ 配置, 可以直接用 VS Code 单独打开该子目录进行
# 开发/调试。
#
# 构建目录说明:
#   整体构建 (本工程)     -> build/<preset>/cppXX/
#   单独打开某个子工程     -> cppXX/build/<preset>/
#   两者是各自独立的构建树, 互不干扰。
# ============================================================================

add_subdirectory(cpp11)
add_subdirectory(cpp14)
add_subdirectory(cpp17)
add_subdirectory(cpp20)
add_subdirectory(cpp23)