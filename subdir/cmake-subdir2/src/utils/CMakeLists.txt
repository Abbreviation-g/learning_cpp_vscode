file(GLOB CURRENT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

target_sources(cmake_subdir2 PRIVATE ${CURRENT_SOURCES})
target_include_directories(cmake_subdir2 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

add_subdirectory(sort)
