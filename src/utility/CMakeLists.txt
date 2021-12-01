set(UTIL_SOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/colour.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/colour.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/direction.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/fastnoise.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/toml.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/toml.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/tomlcpp.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/tomlcpp.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/utfz.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/utfz.h"
)

# add the shared library
add_library(VoxelThingYeah_UTIL STATIC ${UTIL_SOURCES})