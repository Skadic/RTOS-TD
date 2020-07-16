
#include "GameState.h"

#include "GameTasks.h"
#include "../util/GlobalConsts.h"
#include "../util/spawn/EntitySpawn.h"


void GameState::initTasks() {
    addTask(GameTasks::gameRenderTask, "render", DEFAULT_TASK_STACK_SIZE, 0);
    addTask(GameTasks::gameMoveTask, "move", DEFAULT_TASK_STACK_SIZE, 0);
    addTask(GameTasks::gameControlPlayerTask, "control", DEFAULT_TASK_STACK_SIZE, 0);
    addTask(GameTasks::gameMouseInputTask, "mouse", DEFAULT_TASK_STACK_SIZE, 0);
    addTask(GameTasks::gameAITask, "ai", DEFAULT_TASK_STACK_SIZE, 0);
    addTask(GameTasks::gameSpawnTask, "spawn", DEFAULT_TASK_STACK_SIZE, 0);
    addTask(GameTasks::gameTowerTask, "tower", DEFAULT_TASK_STACK_SIZE, 0);
    addTask(GameTasks::gameKillTask, "kill", DEFAULT_TASK_STACK_SIZE, 0);
    addTask(GameTasks::gameWaveTask, "wave", DEFAULT_TASK_STACK_SIZE, 0);
}


GameState::GameState(int mapWidth, int mapHeight) :
        State(),
        map{Map(**registry.lock(portMAX_DELAY), mapWidth, mapHeight)},
        wave(0,1,0, 0)
        {
    renderer.setScale(2);

    spawnPlayer(TilePosition{0, 0}, **registry.lock(portMAX_DELAY));
    initTasks();
    suspendTasks();
}

GameState::GameState(std::string mapPath) :
        State(),
        map{Map(**registry.lock(portMAX_DELAY), mapPath)},
        wave(5,1,0, 0)
        {
    renderer.setScale(2);

    initTasks();
    spawnPlayer(TilePosition{0, 0}, **registry.lock(portMAX_DELAY));

    suspendTasks();
}

Map &GameState::getMap() {
    return map;
}
Wave &GameState::getWave() {
    return wave;
}

void GameState::setWave(const Wave &wave) {
    GameState::wave = wave;
}
