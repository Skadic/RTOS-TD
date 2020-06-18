set(UTIL_SOURCES
        TaskUtils.cpp
        Direction.cpp
        Semaphore.cpp
        Renderer.cpp)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${UTIL_SOURCES})