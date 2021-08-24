add_subdirectory(utility)
add_subdirectory(world)
add_subdirectory(client)
add_subdirectory(server)
add_subdirectory(shared)
add_subdirectory(entities)

set(gameClient_SRCS
	${utility_SRCS}
	${world_SRCS}
	${client_SRCS}
	${shared_SRCS}
	${ent_SRCS}

	PARENT_SCOPE
)

set(gameServer_SRCS
	${utility_SRCS}
	${world_SRCS}
	${server_SRCS}
	${shared_SRCS}
	${ent_SRCS}

	PARENT_SCOPE
)