add_subdirectory(network)
add_subdirectory(sdlstuff)
add_subdirectory(rendering)
add_subdirectory(gui)

set(CLIENT_SOURCES
    ${net_SRCS}
    ${sdlstuff_SRCS}
    ${rendering_SRCS}
    ${gui_SRCS}

	"${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/cvar_clientside.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/inputmanager.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/rendering/glad.c"

	PARENT_SCOPE
)

set(GUI_TEST_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
    PARENT_SCOPE
)