add_subdirectory(network)
add_subdirectory(sdlstuff)
add_subdirectory(gui)
add_subdirectory(world)
add_subdirectory(entities)

set(CLIENT_SOURCES
    ${net_SRCS}
    ${sdlstuff_SRCS}
    ${gui_SRCS}
    ${world_SRCS}
    ${ent_SRCS}

	"${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/cvar_clientside.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/inputmanager.cpp"

	PARENT_SCOPE
)

set(GUI_TEST_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
    PARENT_SCOPE
)