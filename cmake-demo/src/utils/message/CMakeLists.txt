file(GLOB MESSAGE_UTILS_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

add_library(message_utils STATIC
    ${MESSAGE_UTILS_SOURCES}
)
target_include_directories(message_utils
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
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
