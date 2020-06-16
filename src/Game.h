#pragma once

#include <entity/registry.hpp>
#include "states/StateMachine.h"
#include "util/Mutex.h"

extern "C" {
#include <FreeRTOS.h>
#include <semphr.h>
}

inline const unsigned int TARGET_FPS = 144;
inline const unsigned int FRAME_TIME_MS = 1000 / TARGET_FPS;

inline const auto DEFAULT_STACK_SIZE = 100;

class Game {

private:
    SemaphoreHandle_t screenLock;
    SemaphoreHandle_t swapBufferSignal;
    SemaphoreHandle_t drawSignal;
    SemaphoreHandle_t drawHitboxSignal;

    Mutex<entt::registry> registry;

    StateMachine stateMachine;

    Game();
    ~Game();

public:
    static Game& get();

    void start(char* binPath);

    SemaphoreHandle_t getScreenLock();
    SemaphoreHandle_t getSwapBufferSignal();
    SemaphoreHandle_t getDrawSignal();
    SemaphoreHandle_t getDrawHitboxSignal();

    Mutex<entt::registry> &getRegistry();
    StateMachine &getStateMachine();

    Game(Game const&)               = delete;
    void operator=(Game const&)     = delete;
};

void swapBufferTask(void *ptr);

void stateMachineTask(void *stateMachinePointer);