# Keep sorting algorithms as an independent utility module.
file(GLOB SORT_ALGORITHM_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cxx
)

add_library(sort_algorithms STATIC
    ${SORT_ALGORITHM_SOURCES}
)

target_include_directories(sort_algorithms
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
