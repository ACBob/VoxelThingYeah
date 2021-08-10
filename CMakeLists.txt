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

# TODO: windows :trollface:
find_package(SDL2 REQUIRED)

# LodePNG
add_library(lodepng STATIC subprojects/lodepng/lodepng.cpp subprojects/lodepng/lodepng.h)

# TODO: Have to add physfs first because GLM has an if guard around an uninstall target
# EXCLUDE_FROM_ALL isn't working for me for whatever reason

# PhysFS
set(PHYSFS_BUILD_SHARED FALSE)
set(PHYSFS_BUILD_TEST FALSE)
add_subdirectory(subprojects/physfs EXCLUDE_FROM_ALL)
# GLM
add_subdirectory(subprojects/glm EXCLUDE_FROM_ALL)

# Game Sources
add_subdirectory(src)

# Includes
set(includes
	${CMAKE_CURRENT_LIST_DIR}/src
)
include_directories(${includes} ${SDL2_INCLUDE_DIRS} "${CMAKE_CURRENT_LIST_DIR}/subprojects/lodepng" "${CMAKE_CURRENT_LIST_DIR}/subprojects/physfs/src" "${CMAKE_CURRENT_LIST_DIR}/include")


add_executable(${PROJECT_NAME} ${game_SRCS})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} ${CMAKE_DL_LIBS} lodepng GL glm::glm physfs-static)