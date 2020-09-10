# Antibody

Our game is a new interpretation of the classic tower defense genre, based on the FreeRTOS emulator.

Tower defense is a subgenre of real time strategy game. The goal of a regular tower defense game is to defend an objective by preventing the enemies to get there. This can be achieved by building various types of defense system on the map. In most games these defense systems are some kind of tower, which gives this genre its iconic name.

The main threats are the recurring waves of enemies. These waves appear on a specific location on the map and can vary in size. Additional to the increasing size of the wave, the individual enemies are also getting stronger. Nevertheless in most games the enemies are not able to damage or destroy the players defense systems. 

The game is over when a specific amount of enemies have reached the objective.

Therefore the player has to arm himself against the increasing dangers. His arsenal of various types of defense system makes this possible. Each defense system costs a specific amount of ingame-currency and has unique attributes (e.g. range, damage, area of effect). The strategical placement of these systems is the crucial part to win the game. Even tho winning the game is not always possible as it most tower defense games have an infinite amount of waves.

In our version of this game, we adopted the critical parts of the tower defense genre to keep its identity. But at the same time we tried to mix things up by tearing down some boundaries. Instead of being a godlike creature which looks down from above, we decided to visualise the player in the middle of the screen. This player can move though the map, the same way as enemies can do. He is also the one in charge to build the defense systems. 

We tried to open up the harsh restrictions of a static map by making the map completely interactive and way more dynamic. Normally the player would have to place his defense systems along the enemies path. In our game we designed the enemies path to be dynamic. This way the player can create the enemies path by himself. To do so we provided walls and towers to create obstacles for the enemies. Because of that we rather made our game feel like a sandbox than an actual tower defense game. 


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
