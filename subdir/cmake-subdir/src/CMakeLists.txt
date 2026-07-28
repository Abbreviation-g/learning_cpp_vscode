# 先进入库模块目录，确保后续可执行程序能链接这些 target。
add_subdirectory(utils)
add_subdirectory(linked_list)

# 主程序入口，只包含 main.cpp。
add_executable(cmake_subdir_demo
    main.cpp
)

# 主程序直接依赖消息/排序工具库和链表工具库。
target_link_libraries(cmake_subdir_demo
    PRIVATE
        message_utils
        linked_list_utils
)

# 安装可执行程序，并加入统一的导出集合。
install(TARGETS cmake_subdir_demo
    EXPORT cmake_subdir_demoTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# 导出所有已安装 target，方便其他 CMake 工程复用这些库。
install(EXPORT cmake_subdir_demoTargets
    FILE cmake_subdir_demoTargets.cmake
    NAMESPACE cmake_subdir_demo::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cmake_subdir_demo
)
