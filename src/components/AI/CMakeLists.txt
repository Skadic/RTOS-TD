set(AI_SOURCES
        AI.cpp
        MoveTowardsPlayerAI.cpp
        AStar.cpp
        PathfindToNexusAI.cpp)

add_subdirectory(tower)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${AI_SOURCES})