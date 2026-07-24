cmake_minimum_required(VERSION 3.16)
project(cmake-static-lib VERSION 1.0.0 LANGUAGES CXX)

# 所有库目标都使用 C++17，并禁用编译器特有的语言扩展。
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 叶子目录会在目标的安装接口中使用标准安装目录变量，因此必须先加载。
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_subdirectory(src)

# 公开头文件安装到独立目录，避免与动态库示例的头文件发生冲突。
install(
    FILES
        src/utils/message/message.h
        src/utils/sort/sort_algorithms.h
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/cmake-static-lib"
)

# STATIC 目标只安装归档文件，不产生或安装本项目的运行时 DLL。
install(
    TARGETS message_utils sort_utils
    EXPORT cmake-static-lib-targets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

# 导出带命名空间的目标，供消费端通过 find_package() 使用。
install(
    EXPORT cmake-static-lib-targets
    FILE cmake-static-lib-targets.cmake
    NAMESPACE cmake-static-lib::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/cmake-static-lib"
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake-static-lib-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake-static-lib-config.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/cmake-static-lib"
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/cmake-static-lib-config-version.cmake"
    VERSION "${PROJECT_VERSION}"
    COMPATIBILITY SameMajorVersion
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/cmake-static-lib-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake-static-lib-config-version.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/cmake-static-lib"
)
