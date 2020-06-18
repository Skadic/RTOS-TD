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

    LockGuard<std::stack<std::unique_ptr<State>>> stateStack;

public:
    StateMachine();
    StateMachine(const StateMachine &) = delete;

    void pushStack(State *state);
    void popStack();

    State& activeState();
};
