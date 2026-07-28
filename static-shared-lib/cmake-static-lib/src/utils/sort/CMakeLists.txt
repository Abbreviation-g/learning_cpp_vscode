add_library(sort_utils STATIC
    sort_algorithms.cpp
    sort_algorithms.h
)

# 构建树直接使用当前源码目录，安装包则从标准 include 目录查找头文件。
target_include_directories(sort_utils
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
