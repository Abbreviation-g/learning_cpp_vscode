file(GLOB SORT_UTILS_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx"
)

set(CUR_TARGET_NAME my_sort_static_lib)

add_library(${CUR_TARGET_NAME} STATIC
    ${SORT_UTILS_SOURCES}
)

# 跨平台安装目录变量：BINDIR / LIBDIR / INCLUDEDIR / DATAROOTDIR
# https://cmake.org/cmake/help/latest/module/GNUInstallDirs.html
include(GNUInstallDirs)

# 构建期用源码目录，安装后用标准 include 路径
target_include_directories(${CUR_TARGET_NAME}
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

# 1. 安装静态库 + 导出目标信息
install(
    TARGETS ${CUR_TARGET_NAME}
    EXPORT ${CUR_TARGET_NAME}-targets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

# 2. 安装 CMake 自动生成的导入目标文件
install(
    EXPORT ${CUR_TARGET_NAME}-targets
    FILE ${CUR_TARGET_NAME}-targets.cmake
    NAMESPACE ${CUR_TARGET_NAME}::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${CUR_TARGET_NAME}"
)

# 3. 安装手写的包配置文件，消费端通过 find_package() 加载
install(
    FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/${CUR_TARGET_NAME}-config.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${CUR_TARGET_NAME}"
)

# 4. 安装公开头文件到带命名空间的子目录
install(
    FILES
        sort_algorithms.h
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${CUR_TARGET_NAME}"
)