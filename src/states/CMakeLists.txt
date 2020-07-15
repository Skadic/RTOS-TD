set(STATES_SOURCES
        GameState.cpp
        State.cpp
        StateMachine.cpp)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${STATES_SOURCES})