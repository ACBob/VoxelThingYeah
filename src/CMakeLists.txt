add_subdirectory(rendering)
add_subdirectory(sdlstuff)
add_subdirectory(utility)
add_subdirectory(world)
add_subdirectory(gui)
add_subdirectory(sound)
add_subdirectory(client)
add_subdirectory(server)

set(gameClient_SRCS
	${rendering_SRCS}
	${sdlstuff_SRCS}
	${utility_SRCS}
	${world_SRCS}
	${gui_SRCS}
	${sound_SRCS}
	${client_SRCS}

	${CMAKE_CURRENT_SOURCE_DIR}/inputmanager.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/player.cpp
	PARENT_SCOPE
)

set(gameServer_SRCS
	${utility_SRCS}
	${world_SRCS}
	${server_SRCS}

	${CMAKE_CURRENT_SOURCE_DIR}/player.cpp
	PARENT_SCOPE
)