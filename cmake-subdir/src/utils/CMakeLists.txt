# sort 是 utils 的子模块，独立生成 sort_algorithms 静态库。
add_subdirectory(sort)

# Collect only this directory's utility sources. Submodules are managed by
# their own CMakeLists.txt files.
file(GLOB MESSAGE_UTILS_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cxx
)

add_library(message_utils STATIC
    ${MESSAGE_UTILS_SOURCES}
)

# BUILD_INTERFACE 用于源码树构建，INSTALL_INTERFACE 用于安装后的导出 target。
target_include_directories(message_utils
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/cmake_subdir_demo/utils>
)

target_link_libraries(message_utils
    PUBLIC
        sort_algorithms
)

# 安装静态库产物，并加入统一的导出集合。
install(TARGETS message_utils
    EXPORT cmake_subdir_demoTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# 安装对外公开的头文件。
install(FILES message.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cmake_subdir_demo/utils
)
