file(GLOB SORT_UTILS_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx"
)

add_library(my_sort_static_lib STATIC
    ${SORT_UTILS_SOURCES}
)

#https://cmake.org/cmake/help/latest/module/GNUInstallDirs.html
include(GNUInstallDirs)
# 常见变量：
# ${CMAKE_INSTALL_BINDIR}    → bin
# ${CMAKE_INSTALL_LIBDIR}    → lib / lib64
# ${CMAKE_INSTALL_INCLUDEDIR} → include
# ${CMAKE_INSTALL_DATAROOTDIR} → share

# https://cmake.org/cmake/help/latest/command/install.html#targets
INSTALL(TARGETS my_sort_static_lib
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

INSTALL(FILES sort_algorithms.h 
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

INSTALL(FILES ../readme.md
    DESTINATION ${CMAKE_INSTALL_INFODIR}
)
