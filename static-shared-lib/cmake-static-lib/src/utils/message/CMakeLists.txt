add_library(message_utils STATIC
    message.cpp
    message.h
)

# 构建树直接使用当前源码目录，安装包则从标准 include 目录查找头文件。
target_include_directories(message_utils
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
