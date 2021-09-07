add_subdirectory(network)

set(server_SRCS
	${net_SRCS}

	
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/cvar_serverside.cpp
	PARENT_SCOPE
)