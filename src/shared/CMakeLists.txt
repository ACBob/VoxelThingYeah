add_subdirectory(world)
add_subdirectory(entities)
add_subdirectory(network)
add_subdirectory(inventory)
add_subdirectory(blocks)
add_subdirectory(localization)

set(shared_SRCS
	${ent_SRCS}
	${world_SRCS}
	${network_SRCS}
	${inventory_SRCS}
	${block_SRCS}
	${localization_SRCS}

	${CMAKE_CURRENT_SOURCE_DIR}/filesystem.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/cvar.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/blockdef.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/statemanager.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/cvar_shared.cpp
	PARENT_SCOPE
)

set(shared_minimal_SRCS
	${localization_SRCS}

	${CMAKE_CURRENT_SOURCE_DIR}/filesystem.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/cvar.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/blockdef.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/statemanager.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/cvar_shared.cpp
	PARENT_SCOPE
)