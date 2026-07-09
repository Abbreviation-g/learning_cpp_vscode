file(GLOB CURRENT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

target_sources(${PROJECT_NAME} PRIVATE ${CURRENT_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

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
