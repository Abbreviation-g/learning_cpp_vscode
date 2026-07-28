file(GLOB SORT_UTILS_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

add_library(sort_utils SHARED
    ${SORT_UTILS_SOURCES}
)
target_include_directories(sort_utils
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# 自动扫描并添加所有子文件夹
file(GLOB ALL_ENTRIES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*"
)

foreach(ENTRY ${ALL_ENTRIES})
    if(IS_DIRECTORY ${ENTRY})
        get_filename_component(DIR_NAME ${ENTRY} NAME)
        add_subdirectory(${DIR_NAME})
    endif()
endforeach()
