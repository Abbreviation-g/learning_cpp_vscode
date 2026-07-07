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
