# Keep sorting algorithms as an independent utility module.
# 扫描当前 sort 目录中的源文件；新增排序实现时无需手工修改文件列表。
file(GLOB SORT_ALGORITHM_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cxx
)

add_library(sort_algorithms STATIC
    ${SORT_ALGORITHM_SOURCES}
)

# 对外公开 sort 目录，调用方可以直接 include "sort_algorithms.h"。
target_include_directories(sort_algorithms
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/cmake_subdir_demo/utils/sort>
)

# 安装排序静态库，并加入统一的导出集合。
install(TARGETS sort_algorithms
    EXPORT cmake_subdir_demoTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# 安装排序模块公开头文件。
install(FILES sort_algorithms.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cmake_subdir_demo/utils/sort
)
