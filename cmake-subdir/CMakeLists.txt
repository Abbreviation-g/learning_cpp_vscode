cmake_minimum_required(VERSION 3.20)

project(cmake_subdir_demo LANGUAGES CXX)

# Use C++11 for compatibility with the current MinGW g++ 6.3.0 toolchain.
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(src)
