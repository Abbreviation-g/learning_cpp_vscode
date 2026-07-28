cmake_minimum_required(VERSION 3.16)
project(cmake-use-lib VERSION 1.0.0 LANGUAGES CXX)

# C++ 标准设置
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 查找 cmake-lib 动态库
# 需要提前编译并安装 cmake-lib，然后通过 CMAKE_PREFIX_PATH 指定安装路径
# 例如：cmake --install <cmake-lib-build-dir> --prefix C:/install/cmake-lib
#      cmake -DCMAKE_PREFIX_PATH=C:/install/cmake-lib ...
find_package(cmake-lib REQUIRED)

# 构建可执行文件
add_executable(cmake-use-lib src/main.cpp)

# 链接动态库
target_link_libraries(cmake-use-lib PRIVATE
    cmake-lib::message_utils
    cmake-lib::sort_utils
)
