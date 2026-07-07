add_subdirectory(sort)

# Collect only this directory's utility sources. Submodules are managed by
# their own CMakeLists.txt files.
file(GLOB MESSAGE_UTILS_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cxx
)

add_library(message_utils STATIC
    ${MESSAGE_UTILS_SOURCES}
)

target_include_directories(message_utils
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(message_utils
    PUBLIC
        sort_algorithms
)
