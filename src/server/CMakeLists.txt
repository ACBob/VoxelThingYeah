add_subdirectory(network)
add_subdirectory(sound)

set(SERVER_SOURCES
	${net_SRCS}
	${sound_SRCS}

	
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/cvar_serverside.cpp
	PARENT_SCOPE
)