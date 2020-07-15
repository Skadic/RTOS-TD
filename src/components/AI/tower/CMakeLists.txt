set(TOWER_SOURCES
        AreaOfEffectTowerAI.cpp
        LaserTowerAI.cpp)


target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${TOWER_SOURCES})