set(UTIL_SOURCES
        TaskUtils.cpp
        Direction.cpp
        Semaphore.cpp)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${UTIL_SOURCES})