add_subdirectory(utility)
add_subdirectory(client)
add_subdirectory(server)
add_subdirectory(shared)
add_subdirectory(render)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

# PhysFS & ENet are included as submodules
set(PHYSFS_BUILD_STATIC OFF)
set(PHYSFS_BUILD_SHARED ON)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/subprojects/physfs" EXCLUDE_FROM_ALL)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/subprojects/enet" EXCLUDE_FROM_ALL)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/subprojects/glm" EXCLUDE_FROM_ALL)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)


add_library(VoxelThingYeahUtil SHARED ${UTIL_SOURCES})
# target_link_libraries(VoxelThingYeahUtil ${PhysFS_LIBRARIES})
target_compile_definitions(VoxelThingYeahUtil PRIVATE TOML_HEADER_ONLY)

add_library(VoxelThingYeahShared SHARED ${SHARED_SOURCES})
target_link_libraries(VoxelThingYeahShared VoxelThingYeahUtil physfs enet)
target_include_directories(VoxelThingYeahShared PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/subprojects/physfs/src" 
	"${CMAKE_CURRENT_SOURCE_DIR}/subprojects/enet/include"
	"${CMAKE_CURRENT_SOURCE_DIR}/../include"
	"${CMAKE_CURRENT_SOURCE_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}/shared"
)
target_compile_definitions(VoxelThingYeahShared PRIVATE TOML_HEADER_ONLY)

add_library(VoxelThingYeahRender SHARED ${RENDER_SOURCES})
target_link_libraries(VoxelThingYeahRender VoxelThingYeahUtil ${OpenGL_LIBRARIES} glm::glm ${Threads_LIBRARIES} ${SDL2_LIBRARIES})
target_compile_definitions(VoxelThingYeahRender PRIVATE TOML_HEADER_ONLY)
target_include_directories(VoxelThingYeahRender PUBLIC
	${SDL2_INCLUDE_DIRS}
	${OpenGL_INCLUDE_DIRS}
	"${CMAKE_CURRENT_SOURCE_DIR}/subprojects/glm/glm"
	"${CMAKE_CURRENT_SOURCE_DIR}/../include"
	"${CMAKE_CURRENT_SOURCE_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}/shared"
	"${CMAKE_CURRENT_SOURCE_DIR}/client"
	"${CMAKE_CURRENT_SOURCE_DIR}/utility"
)

add_executable( "${PROJECT_NAME}_server" ${SERVER_SOURCES} )
target_link_libraries( "${PROJECT_NAME}_server" VoxelThingYeahShared VoxelThingYeahUtil Threads::Threads )
target_include_directories( "${PROJECT_NAME}_server" PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/../include"
	"${CMAKE_CURRENT_SOURCE_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}/shared"
	"${CMAKE_CURRENT_SOURCE_DIR}/server"
	enet
)
target_compile_definitions( "${PROJECT_NAME}_server" PRIVATE TOML_HEADER_ONLY)

add_executable( "${PROJECT_NAME}_client" ${CLIENT_SOURCES} )
target_link_libraries( "${PROJECT_NAME}_client" VoxelThingYeahShared VoxelThingYeahUtil Threads::Threads
	${SDL2_LIBRARIES}
	${CMAKE_DL_LIBS}
	VoxelThingYeahRender
	enet
)
target_include_directories( "${PROJECT_NAME}_client" PUBLIC
	${GLM_INCLUDE_DIRS}
	${SDL2_INCLUDE_DIRS}
	"${CMAKE_CURRENT_SOURCE_DIR}/../include"
	"${CMAKE_CURRENT_SOURCE_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}/shared"
	"${CMAKE_CURRENT_SOURCE_DIR}/client"
	"${CMAKE_CURRENT_SOURCE_DIR}/utility"
)
target_compile_definitions( "${PROJECT_NAME}_client" PRIVATE TOML_HEADER_ONLY)

add_executable( guiTest ${GUI_TEST_SOURCES} )
target_link_libraries( guiTest VoxelThingYeahShared VoxelThingYeahUtil Threads::Threads
	${SDL2_LIBRARIES}
	${CMAKE_DL_LIBS}
	VoxelThingYeahRender
)
target_include_directories( guiTest PUBLIC
	${GLM_INCLUDE_DIRS}
	${SDL2_INCLUDE_DIRS}
	${OpenGL_INCLUDE_DIRS}
	"${CMAKE_CURRENT_SOURCE_DIR}/../include"
	"${CMAKE_CURRENT_SOURCE_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}/shared"
	"${CMAKE_CURRENT_SOURCE_DIR}/client"
	"${CMAKE_CURRENT_SOURCE_DIR}/utility"
)
target_compile_definitions( guiTest PRIVATE TOML_HEADER_ONLY)