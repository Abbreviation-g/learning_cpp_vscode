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
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/cmake_subdir_demo/utils/sort>
)

install(TARGETS sort_algorithms
    EXPORT cmake_subdir_demoTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES sort_algorithms.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cmake_subdir_demo/utils/sort
)
