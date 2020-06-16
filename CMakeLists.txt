project(RTOS_TD)

cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
set(CMAKE_CONFIG_DIR ${PROJECT_SOURCE_DIR}/config)

if("${TOOLCHAIN}" STREQUAL "docker" OR "${TOOLCHAIN}" STREQUAL "vm")
    include(${CMAKE_MODULE_PATH}/init_remote_projects.cmake)
else()
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON )
    set(CMAKE_BUILD_TYPE "Debug")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

    add_compile_options("-Wall" "-O0")

    option(TRACE_FUNCTIONS "Trace function calls using instrument-functions")

    find_package(Threads)
    find_package(SDL2 REQUIRED)
    add_library(sdl2 INTERFACE)
    target_link_libraries(sdl2 INTERFACE ${SDL2_LIBRARIES})
    target_include_directories(sdl2 INTERFACE ${SDL2_INCLUDE_DIRS})

    find_package(SDL2_gfx REQUIRED)
    add_library(sdl2_gfx INTERFACE)
    target_link_libraries(sdl2_gfx INTERFACE ${SDL2_GFX_LIBRARIES})
    target_include_directories(sdl2_gfx INTERFACE ${SDL2_GFX_INCLUDE_DIRS})

    find_package(SDL2_image REQUIRED)
    add_library(sdl2_image INTERFACE)
    target_link_libraries(sdl2_image INTERFACE ${SDL2_IMAGE_LIBRARIES})
    target_include_directories(sdl2_image INTERFACE ${SDL2_IMAGE_INCLUDE_DIRS})

    find_package(SDL2_mixer REQUIRED)
    add_library(sdl2_mixer INTERFACE)
    target_link_libraries(sdl2_mixer INTERFACE ${SDL2_MIXER_LIBRARIES})
    target_include_directories(sdl2_mixer INTERFACE ${SDL2_MIXER_INCLUDE_DIRS})

    find_package(SDL2_ttf REQUIRED)
    add_library(sdl2_ttf INTERFACE)
    target_link_libraries(sdl2_ttf INTERFACE ${SDL2_TTF_LIBRARIES})
    target_include_directories(sdl2_ttf INTERFACE ${SDL2_TTF_INCLUDE_DIRS})


    if(TRACE_FUNCTIONS)
        add_definitions(-DTRACE_FUNCTIONS)
        SET(GCC_COVERAGE_COMPILE_FLAGS "-finstrument-functions")
        target_compile_options(FreeRTOS_Emulator PUBLIC ${GCC_COVERAGE_COMPILE_FLAGS})
    endif(TRACE_FUNCTIONS)

    add_subdirectory(lib)
    add_subdirectory(src)

    set(ENABLE_ASAN OFF)

    include(${CMAKE_MODULE_PATH}/tests.cmake)

    if(DOCS)
        find_package(Doxygen REQUIRED)

        add_custom_target(
                docs
                COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_SOURCE_DIR}/docs/Doxyfile
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/docs
                COMMENT "Generating API documentation with Doxygen"
                VERBATIM
        )
    endif()

    include(${CMAKE_MODULE_PATH}/init_local_projects.cmake)
endif()