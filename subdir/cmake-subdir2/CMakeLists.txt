cmake_minimum_required(VERSION 3.20)

project(cmake_subdir2 LANGUAGES CXX)

add_executable(cmake_subdir2)

target_compile_features(cmake_subdir2 PRIVATE cxx_std_17)

add_subdirectory(src)
