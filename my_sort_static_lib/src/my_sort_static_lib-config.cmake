# 消费端通过 find_package(my_sort_static_lib) 加载本文件。
# 本文件只做一件事：加载 CMake 自动生成的导入目标文件。
include("${CMAKE_CURRENT_LIST_DIR}/my_sort_static_lib-targets.cmake")
