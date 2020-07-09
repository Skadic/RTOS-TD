set(UTIL_SOURCES
        TaskUtils.cpp
        Direction.cpp
        Semaphore.cpp
        Renderer.cpp
        InputHandler.cpp
        Log.cpp
        Map.cpp
        TileType.cpp)

add_subdirectory(spawn)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${UTIL_SOURCES})