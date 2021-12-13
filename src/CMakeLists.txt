add_subdirectory(utility)
add_subdirectory(client)
add_subdirectory(server)
add_subdirectory(shared)

find_package(PhysFS REQUIRED)
find_package(ENet REQUIRED)
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)


add_library(libVoxelThingYeahUtil SHARED ${UTIL_SOURCES})
# target_link_libraries(libVoxelThingYeahUtil ${PhysFS_LIBRARIES})

add_library(libVoxelThingYeahShared SHARED ${SHARED_SOURCES})
target_link_libraries(libVoxelThingYeahShared ${PhysFS_LIBRARIES} ${ENet_LIBRARIES})
target_include_directories(libVoxelThingYeahShared PUBLIC
	${PhysFS_INCLUDE_DIRS} 
	${ENet_INCLUDE_DIRS}
	"${CMAKE_CURRENT_SOURCE_DIR}/../include"
	"${CMAKE_CURRENT_SOURCE_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}/shared"
)