set(UTIL_SOURCES
        TaskUtils.cpp
        Renderer.cpp
        InputHandler.cpp
        Log.cpp
        Wave.cpp
        StringUtils.cpp
        CollisionTable.cpp)

add_subdirectory(spawn)
add_subdirectory(sync)
add_subdirectory(map)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${UTIL_SOURCES})