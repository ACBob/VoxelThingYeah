add_subdirectory(network)

set(CLIENT_SOURCES
    ${net_SRCS}

	"${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/cvar_clientside.cpp"
	PARENT_SCOPE
)

set(GUI_TEST_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
    PARENT_SCOPE
)