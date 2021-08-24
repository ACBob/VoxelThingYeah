add_subdirectory(gui)
add_subdirectory(rendering)
add_subdirectory(sdlstuff)
add_subdirectory(sound)

set(client_SRCS
	${rendering_SRCS}
	${sdlstuff_SRCS}
	${gui_SRCS}
	${sound_SRCS}

	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
	PARENT_SCOPE
)