add_library(message_utils STATIC
    message.cpp
)

target_include_directories(message_utils
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
