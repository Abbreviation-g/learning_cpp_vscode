cmake_minimum_required(VERSION 3.16)
project(cmake-use-static-lib VERSION 1.0.0 LANGUAGES CXX)

# 使用固定的 C++17 标准，确保消费端与示例静态库的语言配置一致。
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 需要先构建并安装 cmake-static-lib，再通过 CMAKE_PREFIX_PATH 指定安装前缀。
# 例如：cmake --install <build-dir> --prefix C:/install/cmake-static-lib
find_package(cmake-static-lib REQUIRED)

add_executable(cmake-use-static-lib src/main.cpp)

# 链接包配置文件导出的两个静态库目标。
target_link_libraries(cmake-use-static-lib PRIVATE
    cmake-static-lib::message_utils
    cmake-static-lib::sort_utils
)
