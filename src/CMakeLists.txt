add_subdirectory(utility)
add_subdirectory(client)
add_subdirectory(server)
add_subdirectory(shared)

find_package(PhysFS REQUIRED)
find_package(ENet REQUIRED)
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)


add_library(libVoxelThingYeahUtil SHARED ${UTIL_SOURCES})
# target_link_libraries(libVoxelThingYeahUtil ${PhysFS_LIBRARIES})

add_library(libVoxelThingYeahShared SHARED ${SHARED_SOURCES})
target_link_libraries(libVoxelThingYeahShared ${PHYSFS_LIBRARY} ${ENET_LIBRARIES})
target_include_directories(libVoxelThingYeahShared PUBLIC
	${PhysFS_INCLUDE_DIRS} 
	${ENet_INCLUDE_DIRS}
	"${CMAKE_CURRENT_SOURCE_DIR}/../include"
	"${CMAKE_CURRENT_SOURCE_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}/shared"
)

add_executable( "${PROJECT_NAME}_server" ${SERVER_SOURCES} )
target_link_libraries( "${PROJECT_NAME}_server" libVoxelThingYeahShared libVoxelThingYeahUtil Threads::Threads )
target_include_directories( "${PROJECT_NAME}_server" PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/../include"
	"${CMAKE_CURRENT_SOURCE_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}/shared"
	"${CMAKE_CURRENT_SOURCE_DIR}/server"
)