set(STATES_SOURCES
        GameState.cpp
        GameTasks.cpp
        State.cpp
        StateMachine.cpp
        RenderTasks.cpp
        MainMenuState.cpp
        MainMenuTasks.cpp)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${STATES_SOURCES})