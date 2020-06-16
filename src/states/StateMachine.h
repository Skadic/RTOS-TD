#pragma once

#include <stack>
#include "State.h"
#include <memory>

extern "C" {
#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>
}

enum StateChange {
    PUSH,
    POP,
    NONE
};

class StateMachine {
private:

    std::stack<std::unique_ptr<State>> stateStack;
    QueueHandle_t nextStateQueue;
    QueueHandle_t operationQueue;
    SemaphoreHandle_t queueLock;
    bool changed;

public:
    StateMachine();

    void enqueuePush(State *state);
    void enqueuePop();

    bool hasChanged();
    State& activeState();

    StateChange handleOperation();
};
