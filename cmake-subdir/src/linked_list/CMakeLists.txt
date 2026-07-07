# Keep linked list code as an independent module under src.
file(GLOB LINKED_LIST_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cxx
)

add_library(linked_list_utils STATIC
    ${LINKED_LIST_SOURCES}
)

target_include_directories(linked_list_utils
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/cmake_subdir_demo/linked_list>
)

install(TARGETS linked_list_utils
    EXPORT cmake_subdir_demoTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES singly_linked_list.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cmake_subdir_demo/linked_list
)
