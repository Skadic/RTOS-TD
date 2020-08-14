set(AI_SOURCES
        AI.cpp
        MoveTowardsPlayerAI.cpp
        AStar.cpp
        WalkPathAI.cpp)

add_subdirectory(tower)
add_subdirectory(projectile)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${AI_SOURCES})