file(GLOB BYTE_DEMO_SOURCES CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_library(byte_demo OBJECT ${BYTE_DEMO_SOURCES})

target_include_directories(byte_demo PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
)
