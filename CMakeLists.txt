# Copied from Platforrmer's 3d fork (Which I will never ever release)
cmake_minimum_required(VERSION 3.16)

project(VoxelThingYeah VERSION 1.0.0)

option(MEEGREEF_ENABLE_OPENAL "Enable OpenAL support" ON)
# If no build type is specified, Build in release.
# if(${CMAKE_BUILD_TYPE})
# else()
# 	set(CMAKE_BUILD_TYPE Release)
# endif()

# Set C flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wno-unused-parameter -Wno-missing-field-initializers")

if (WIN32)
	set(CMAKE_CXX_FLAGS "-static-libgcc -static-libstdc++ ${CMAKE_CXX_FLAGS}")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Put the game executable in /bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# LodePNG
add_library(lodepng STATIC src/subprojects/lodepng/lodepng.cpp src/subprojects/lodepng/lodepng.h)

# Includes
set(includes
	${CMAKE_CURRENT_LIST_DIR}/src
)

configure_file( "${CMAKE_CURRENT_LIST_DIR}/include/config.h.in" "${CMAKE_CURRENT_LIST_DIR}/include/config.h" )

# Game Sources
add_subdirectory(src)

target_link_libraries(
	libVoxelThingYeahRender lodepng
)
target_include_directories(
	libVoxelThingYeahRender PUBLIC "src/subprojects/lodepng/"
)