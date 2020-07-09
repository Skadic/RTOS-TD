set(COMPONENTS_SOURCES
        Hitbox.cpp
        Position.cpp
        Sprites.cpp
        Velocity.cpp
        Health.cpp)

add_subdirectory(AI)
add_subdirectory(tilecomponents)
add_subdirectory(tags)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${COMPONENTS_SOURCES})