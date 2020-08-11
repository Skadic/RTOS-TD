set(STATES_SOURCES
        State.cpp
        StateMachine.cpp
        mainmenu/MainMenuState.cpp
        )

add_subdirectory(gamestate)
add_subdirectory(mainmenu)
add_subdirectory(gameoverstate)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${STATES_SOURCES})