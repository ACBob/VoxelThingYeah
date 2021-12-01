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
	${CMAKE_CURRENT_SOURCE_DIR}/raycast.cpp
)

# We also handle ENet stuff here, so include that too
add_library(VoxelThingYeah_SHARED STATIC ${shared_SRCS})
target_include_directories(
	# Grody, is there a better way to do this?
	VoxelThingYeah_SHARED PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
	VoxelThingYeah_SHARED PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../"
	VoxelThingYeah_SHARED PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../../subprojects/enet/include"
	VoxelThingYeah_SHARED PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../../subprojects/physfs/src"
	VoxelThingYeah_SHARED PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../../include"
)