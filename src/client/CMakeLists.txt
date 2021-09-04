add_subdirectory(gui)
add_subdirectory(rendering)
add_subdirectory(sdlstuff)
add_subdirectory(sound)
add_subdirectory(particles)

set(client_SRCS
	${rendering_SRCS}
	${sdlstuff_SRCS}
	${gui_SRCS}
	${sound_SRCS}
	${particle_SRCS}

	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/cvar_clientside.cpp
	PARENT_SCOPE
)