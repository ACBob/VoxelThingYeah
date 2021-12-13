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

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Put the game executable in /bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)


set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

if (MEEGREEF_ENABLE_OPENAL AND NOT DEFINED OPENAL_INCLUDE_DIRS)
	find_package(OpenAL)
	if (${OPENAL_FOUND})
		add_compile_definitions(MEEGREEF_ENABLE_OPENAL)
	else()
		set(MEEGREEF_ENABLE_OPENAL OFF)
		message(WARNING "Couldn't find OpenAL, Disabling audio support.")
	endif()
elseif(DEFINED OPENAL_INCLUDE_DIRS AND MEEGREEF_ENABLE_OPENAL)
	add_compile_definitions(MEEGREEF_ENABLE_OPENAL)
endif()

# LodePNG
add_library(lodepng STATIC subprojects/lodepng/lodepng.cpp subprojects/lodepng/lodepng.h)

# TODO: Have to add physfs first because GLM has an if guard around an uninstall target
# EXCLUDE_FROM_ALL isn't working for me for whatever reason

# GLM
add_subdirectory(subprojects/glm EXCLUDE_FROM_ALL)

# Includes
set(includes
	${CMAKE_CURRENT_LIST_DIR}/src
)

configure_file( "${CMAKE_CURRENT_LIST_DIR}/include/config.h.in" "${CMAKE_CURRENT_LIST_DIR}/include/config.h" )

# Game Sources
add_subdirectory(src)