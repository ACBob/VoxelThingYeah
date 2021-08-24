add_subdirectory(world)
add_subdirectory(entities)

set(shared_SRCS
	${ent_SRCS}
	${world_SRCS}

	${CMAKE_CURRENT_SOURCE_DIR}/filesystem.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/network.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/cvar.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/inputmanager.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/blockdef.cpp
	PARENT_SCOPE
)