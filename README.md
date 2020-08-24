# Antibody

A tower defense game, based on the FreeRTOS emulator.

## Dependencies

### [FreeRTOS-Emulator](https://github.com/alxhoff/FreeRTOS-Emulator)

This project is based on the FreeRTOS Emulator and its included SDL2 based drawing capabilities. It is noted however that the drawing functionality has been extended by us to allow to draw e.g. non-filled circles. See the [TUM_Draw header file](https://github.com/Skadic/RTOS-TD/blob/master/lib/Gfx/include/TUM_Draw.h) for the functions offered here.

In addition: To add custom sound samples place them in the `resources/waveforms` folder and add the sample name to the `FOR_EACH_SAMPLE` macro in the [TUM_Sound header file](https://github.com/Skadic/RTOS-TD/blob/master/lib/Gfx/include/TUM_Sound.h).

### [entt](https://github.com/skypjack/entt)

**entt** is the [entity-component-system](https://en.wikipedia.org/wiki/Entity_component_system) (ECS for short) that in conjunction with the FreeRTOS Emulators' task scheduling is driving most of the game's functionality.

In entt, the entities with their components are stored in an `entt::registry` object. It also importantly allows efficient methods of accessing them by way of views (examples can be found in the readme of the linked github repository).  

In this game, each state has its own set of tasks, each task (which accesses the registry) resembling a system in the ECS. For examples, see the [GameTasks](https://github.com/Skadic/RTOS-TD/blob/master/src/states/gamestate/GameTasks.cpp) file.

### [json](https://github.com/nlohmann/json)

**json** is used for parsing the map from json files.

## Other resources

### [sfxr](https://www.drpetter.se/project_sfxr.html)

An excellent tool to create sound effects for video games. **sfxr** was used to create all sounds for this game.