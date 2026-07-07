add_subdirectory(utils)
add_subdirectory(linked_list)

add_executable(cmake_subdir_demo
    main.cpp
)

target_link_libraries(cmake_subdir_demo
    PRIVATE
        message_utils
        linked_list_utils
)

install(TARGETS cmake_subdir_demo
    EXPORT cmake_subdir_demoTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(EXPORT cmake_subdir_demoTargets
    FILE cmake_subdir_demoTargets.cmake
    NAMESPACE cmake_subdir_demo::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cmake_subdir_demo
)
