set(SIMULATOR_SOURCES "main.c")
set(GAME_SOURCES "Main.cpp" "Game.cpp")

SET(PROJECT_SOURCES
        ${SIMULATOR_SOURCES}
        ${GAME_SOURCES} )

add_executable(${CMAKE_PROJECT_NAME} ${PROJECT_SOURCES})

add_subdirectory(components)
add_subdirectory(states)
add_subdirectory(util)

target_link_libraries(${CMAKE_PROJECT_NAME} freertos gfx async tracer entt glm m json)

if(ENABLE_ASAN)
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -g -fsanitize=address -static-libasan -fno-omit-frame-pointer -O0)
    target_link_libraries(${CMAKE_PROJECT_NAME} -fsanitize=address)
endif()

