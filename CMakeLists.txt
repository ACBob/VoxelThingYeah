# Copied from Platforrmer's 3d fork (Which I will never ever release)
cmake_minimum_required(VERSION 3.16)

project(VoxelThingYeah VERSION 0.0.0)

# If no build type is specified, Build in release.
# if(${CMAKE_BUILD_TYPE})
# else()
# 	set(CMAKE_BUILD_TYPE Release)
# endif()

# Set C flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wno-unused-parameter -Wno-missing-field-initializers -fno-exceptions")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Put the game executable in /bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_subdirectory(subprojects/bgfx.cmake)

# TODO: windows :trollface:
find_package(SDL2 REQUIRED)

# LodePNG
add_library(lodepng STATIC subprojects/lodepng/lodepng.cpp subprojects/lodepng/lodepng.h)


# Includes
set(includes
	.
)
list(TRANSFORM includes PREPEND "src/")
include_directories(${includes} ${SDL2_INCLUDE_DIRS} subprojects/lodepng)

set(sources
	main.cpp
	vector.cpp
	player.cpp
	inputmanager.cpp
	chunk.cpp
	chunkmodel.cpp
	block.cpp
	chunkmanager.cpp

	sdlbgfx/sdlwindow.cpp
	sdlbgfx/bgfxtexturemanager.cpp
	sdlbgfx/bgfxchunkrenderer.cpp
)
list(TRANSFORM sources PREPEND "src/")

add_executable(${PROJECT_NAME} ${sources})
target_link_libraries(${PROJECT_NAME} bx bgfx ${SDL2_LIBRARIES} ${CMAKE_DL_LIBS} lodepng)