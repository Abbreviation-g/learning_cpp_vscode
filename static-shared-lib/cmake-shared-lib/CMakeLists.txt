cmake_minimum_required(VERSION 3.16)
project(cmake-lib VERSION 1.0.0 LANGUAGES CXX)

# C++ 标准设置
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(src)

# ===== 安装与导出规则 =====
# 使其他 CMake 工程可以通过 find_package(cmake-lib) 使用本库
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# 安装头文件到 include/cmake-lib/ 目录
install(FILES
    src/utils/message/message.h
    src/utils/sort/sort_algorithms.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cmake-lib
)

# 安装库目标，并加入导出集 cmake-lib-targets
# INCLUDES DESTINATION 让使用者自动获得 include 路径
install(
    TARGETS message_utils sort_utils
    EXPORT cmake-lib-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# 安装导出集（targets 文件）
install(
    EXPORT cmake-lib-targets
    FILE cmake-lib-targets.cmake
    NAMESPACE cmake-lib::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cmake-lib
)

# 生成并安装包配置文件与版本文件
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake-lib-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cmake-lib-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cmake-lib
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/cmake-lib-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/cmake-lib-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/cmake-lib-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cmake-lib
)
