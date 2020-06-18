set(UTIL_SOURCES
        TaskUtils.cpp
        Direction.cpp
        Semaphore.cpp
        Renderer.cpp
        InputHandler.cpp)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${UTIL_SOURCES})