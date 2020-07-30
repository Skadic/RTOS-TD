set(TOWER_SOURCES
        AreaOfEffectTowerAI.cpp
        LaserTowerAI.cpp
        ProjectileTowerAI.cpp)


target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${TOWER_SOURCES})