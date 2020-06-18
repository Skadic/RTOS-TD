#pragma once

#include <entity/registry.hpp>
#include "states/StateMachine.h"
#include "util/LockGuard.h"

extern "C" {
#include <FreeRTOS.h>
#include <semphr.h>
}

class Game {

private:
    Semaphore screenLock;
    Semaphore drawSignal;
    Semaphore drawHitboxSignal;
    Semaphore swapBufferSignal;
    StateMachine stateMachine;

    Game();
    ~Game();

public:

    static Game& get();
    void start(char* binPath);

    Semaphore &getScreenLock();

    Semaphore &getDrawSignal();
    Semaphore &getDrawHitboxSignal();
    Semaphore &getSwapBufferSignal();
    StateMachine &getStateMachine();

    LockGuard<entt::registry> &getActiveStateRegistry();

    Game(Game const&)               = delete;
    void operator=(Game const&)     = delete;
};

void swapBufferTask(void *ptr);
void stateMachineTask(void *ptr);
void inputTask(void *ptr);