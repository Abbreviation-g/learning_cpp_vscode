cmake_minimum_required(VERSION 3.16)
project(cpp11 VERSION 1.0.0 LANGUAGES CXX)

# https://en.cppreference.com/cpp/11
# ============================================================================
# 独立的 C++11 工程
#
# 本工程只演示 C++11 特性, 编译标准固定为 C++11。
# 每个 <feature>/main.cpp 都是一个独立的可执行 target。
#
# 构建 (在本目录下):
#   cmake --preset mingw
#   cmake --build --preset mingwbuild
# 产物: build/mingw/bin/<target>[.exe]
# ============================================================================

# C++ 标准设置
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# 将头文件搜索路径直接写入 compile_commands.json，便于 clangd 读取。
set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_INCLUDES OFF)

# 输出目录设置
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 查找线程库(MinGW需要)
find_package(Threads REQUIRED)

# c++11 constexpr
add_executable(cpp11_mysql src/main.cpp)

# MySQL Connector/C++ 头文件目录
target_include_directories(cpp11_mysql PRIVATE
	"C:/Program Files/MySQL/MySQL Server 8.0/include"
)

# MySQL 导入库所在目录
target_link_directories(cpp11_mysql PRIVATE
	"C:/Program Files/MySQL/MySQL Server 8.0/lib"
)

# 链接 MySQL 客户端导入库
target_link_libraries(cpp11_mysql PRIVATE
	libmysql.lib
)

# 将 MySQL 客户端运行时 DLL 复制到可执行文件目录，便于直接运行程序。
add_custom_command(TARGET cpp11_mysql POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"C:/Program Files/MySQL/MySQL Server 8.0/lib/libmysql.dll"
		"$<TARGET_FILE_DIR:cpp11_mysql>"
)
