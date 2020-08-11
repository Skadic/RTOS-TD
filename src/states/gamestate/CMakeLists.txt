set(GAME_STATE_SOURCES
        GameState.cpp
        GameTasks.cpp
        RenderTasks.cpp)


target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${GAME_STATE_SOURCES})