add_subdirectory(gui)
add_subdirectory(rendering)
add_subdirectory(sdlstuff)
add_subdirectory(particles)
add_subdirectory(network)

if (${MEEGREEF_ENABLE_OPENAL})
	add_subdirectory(sound)
else()
	set(sound_SRCS "")
endif()

set(client_SRCS
	${rendering_SRCS}
	${sdlstuff_SRCS}
	${gui_SRCS}
	${sound_SRCS}
	${particle_SRCS}
	${net_SRCS}

	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/cvar_clientside.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/gamestates.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/packs.cpp
	PARENT_SCOPE
)