add_subdirectory(jeneration)

set(world_SRCS
	${jeneration_SRCS}

	${CMAKE_CURRENT_SOURCE_DIR}/chunk.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/world.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/physics.cpp
	PARENT_SCOPE
)