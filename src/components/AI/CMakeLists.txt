set(AI_SOURCES
        AI.cpp
        MoveTowardsPlayerAI.cpp
        AStar.cpp
        PathfindToNexusAI.cpp)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${AI_SOURCES})