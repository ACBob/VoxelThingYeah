add_subdirectory(utility)
add_subdirectory(client)
add_subdirectory(server)
add_subdirectory(shared)

set(gameClient_SRCS
	${utility_SRCS}
	${client_SRCS}
	${shared_SRCS}

	PARENT_SCOPE
)

set(guiTest_SRCS
	${guiTest_SRCS}
	${utility_minimal_SRCS}
	${shared_minimal_SRCS}
	PARENT_SCOPE
)

set(gameServer_SRCS
	${utility_SRCS}
	${server_SRCS}
	${shared_SRCS}

	PARENT_SCOPE
)