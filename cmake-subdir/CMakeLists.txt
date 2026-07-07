cmake_minimum_required(VERSION 3.20)

project(cmake_subdir_demo LANGUAGES CXX)

# 引入标准安装目录变量，例如 CMAKE_INSTALL_BINDIR/LIBDIR/INCLUDEDIR。
include(GNUInstallDirs)

# Use C++11 for compatibility with the current MinGW g++ 6.3.0 toolchain.
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 进入源码目录，具体 target 在 src 及其子目录中定义。
add_subdirectory(src)

# 根据模板生成构建目录中的卸载脚本，脚本会读取 install_manifest.txt。
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
    @ONLY
)

# 创建自定义卸载目标：cmake --build --preset mingw --target uninstall。
add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
)
