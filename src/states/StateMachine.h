#pragma once

#include <stack>
#include "State.h"
#include <memory>

extern "C" {
#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>
}

/**
 * All the state changes that can be enqueued to the statemachine
 */
enum StateChange {
    /**
     * Push a state to the stack
     */
    PUSH,
    /**
     * Pop a state off the stack
     */
    POP,
    /**
     * Pop 2 states off the stack
     * This is used for when a state needs to pop the stack under it as well, as in the case of the Game Over state,
     * which needs to remove the Game State below it as well.
     */
    POP2X,
    /**
     * No state change
     */
    NONE
};

class StateMachine {
private:
    /**
     * A stack of states, guarded by a LockGuard
     * The active state is always the state on top of the stack
     */
    LockGuard<std::stack<std::unique_ptr<State>>> stateStack;

public:
    /**
     * Creates an empty state machine
     */
    StateMachine();

    /**
     * The state machine cannot be copied
     */
    StateMachine(const StateMachine &) = delete;

    /**
     * Pushes a state to the stack, suspends the tasks of the previous active state, and runs the tasks of the newly pushed state.
     * @param state The state to be pushed. It should be created with "new", as the state machine handles cleanup
     */
    void pushStack(State* state);

    /**
     * Removes the stack from the top off the stack, and resumes the tasks of the state below it, if there is one.
     */
    void popStack();

    /**
     * Removes 2 stacks from the top off the stack, and resumes the tasks of the state below them, if there is one.
     */
    void popStack2X();

    /**
     * Whether the stack is empty or not.
     */
    bool empty();

    /**
     * Returns a reference to the currently active state (i.e. the state on top of the stack)
     */
    State& activeState();
};
