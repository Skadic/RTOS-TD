set(STATES_SOURCES
        State.cpp
        StateMachine.cpp
        )

add_subdirectory(gamestate)
add_subdirectory(mainmenu)
add_subdirectory(gameoverstate)
add_subdirectory(tutorial)
add_subdirectory(abstractmenustate)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${STATES_SOURCES})