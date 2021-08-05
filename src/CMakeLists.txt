add_subdirectory(rendering)
add_subdirectory(sdlstuff)
add_subdirectory(utility)
add_subdirectory(world)
add_subdirectory(gui)

set(game_SRCS
	${rendering_SRCS}
	${sdlstuff_SRCS}
	${utility_SRCS}
	${world_SRCS}
	${gui_SRCS}

	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/inputmanager.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/player.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/textrender.cpp
	PARENT_SCOPE
)