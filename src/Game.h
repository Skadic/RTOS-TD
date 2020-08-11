#pragma once

#include <entity/registry.hpp>
#include "states/StateMachine.h"
#include "util/sync/LockGuard.h"
#include "util/InputHandler.h"

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
    LockGuard<InputHandler> buttons;
    StateMachine stateMachine;
    LockGuard<State*> stateToPush;
    LockGuard<StateChange> stateOperation;

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

    LockGuard<State*> &getStateToPush();
    LockGuard<StateChange> &getStateOperation();

    void enqueueStatePush(State* state);
    void enqueueStatePop();
    void enqueueStatePop2X();

    LockGuard<entt::registry> &getActiveStateRegistry();
    LockGuard<InputHandler> &getInput();

    Game(Game const&)               = delete;
    void operator=(Game const&)     = delete;
};

void swapBufferTask(void *ptr);
void stateMachineTask(void *ptr);
void inputTask(void *ptr);