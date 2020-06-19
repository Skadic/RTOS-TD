

file(GLOB FREERTOS_SOURCES
        "FreeRTOS_Kernel/*.c"
        "FreeRTOS_Kernel/portable/GCC/Posix/*.c"
        "FreeRTOS_Kernel/portable/MemMang/*.c")
add_library(freertos STATIC ${FREERTOS_SOURCES})
target_include_directories(freertos PUBLIC
        FreeRTOS_Kernel/include
        FreeRTOS_Kernel/portable/GCC/Posix
        ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(freertos Threads::Threads)

file(GLOB GFX_SOURCES "Gfx/*.c")
add_library(gfx STATIC ${GFX_SOURCES})
target_include_directories(gfx PUBLIC
        Gfx/include)
target_link_libraries(gfx
        freertos
        sdl2
        sdl2_gfx
        sdl2_image
        sdl2_mixer
        sdl2_ttf
        m
        Threads::Threads)

file(GLOB ASYNC_SOURCES "AsyncIO/*.c")
add_library(async STATIC ${ASYNC_SOURCES})
target_include_directories(async PUBLIC
        AsyncIO/include)
target_link_libraries(async Threads::Threads)

add_library(tracer INTERFACE)
target_include_directories(tracer INTERFACE tracer/include)

add_library(entt INTERFACE)
target_include_directories(entt INTERFACE entt)

add_library(glm INTERFACE)
target_include_directories(glm INTERFACE
        glm
        glm/ext)
target_link_libraries(glm INTERFACE m)

add_library(json INTERFACE)
target_include_directories(json INTERFACE json/include)