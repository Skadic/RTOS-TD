set(COMPONENTS_SOURCES
        Hitbox.cpp
        Position.cpp
        Sprites.cpp
        Velocity.cpp
        TilePosition.cpp
        Health.cpp)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${COMPONENTS_SOURCES})