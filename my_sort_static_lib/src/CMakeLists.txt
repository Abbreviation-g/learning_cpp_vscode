file(GLOB SORT_UTILS_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx"
)

add_library(my_sort_static_lib STATIC
    ${SORT_UTILS_SOURCES}
)

# https://cmake.org/cmake/help/latest/command/install.html#targets
INSTALL(TARGETS my_sort_static_lib
    ARCHIVE DESTINATION lib
)

INSTALL(FILES sort_algorithms.h 
    DESTINATION include
)

INSTALL(FILES ../readme.md
    DESTINATION doc
)
