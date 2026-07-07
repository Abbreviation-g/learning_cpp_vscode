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
        ${CMAKE_CURRENT_SOURCE_DIR}
)
