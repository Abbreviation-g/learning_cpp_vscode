cmake_minimum_required(VERSION 3.14)
project(cpp_native_sort_int_arr LANGUAGES CXX)


set(CUR_TARGET_NAME ${PROJECT_NAME})

# 创建动态库目标
add_library(${CUR_TARGET_NAME} SHARED
    src/sort_int_arr.cpp
)

# 定义导出宏，启用 __declspec(dllexport)
target_compile_definitions(${CUR_TARGET_NAME} PRIVATE CPP_NATIVE_EXPORTS)

