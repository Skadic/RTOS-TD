#pragma once

namespace GameTasks {
    void gameRenderTask(void *statePointer);
    void gameMoveTask(void *statePointer);
    void gameControlPlayerTask(void *statePointer);
    void gameMouseInputTask(void *statePointer);
    void gameAITask(void *statePointer);
    void gameSpawnTask(void *statePointer);
    void gameTowerTask(void *statePointer);
    void gameKillTask(void *statePointer);
}