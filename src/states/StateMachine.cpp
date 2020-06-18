//
// Created by skadic on 16.06.20.
//

#include "StateMachine.h"

#include "State.h"
#include <iostream>

StateMachine::StateMachine() : stateStack{std::stack<std::unique_ptr<State>>(), xSemaphoreCreateMutex()} {
    std::cout << "Created State Machine" << std::endl;
    queueLock = xSemaphoreCreateMutex();
    nextStateQueue = xQueueCreate(3, sizeof(State*));
    operationQueue = xQueueCreate(3, sizeof(StateChange));
    changed = false;
}

bool StateMachine::hasChanged() {
    return this->changed;
}

State &StateMachine::activeState() {
    return *(*stateStack.lock(portMAX_DELAY))->top();
}

void StateMachine::popStack() {

    if (auto stackOpt = stateStack.lock(portMAX_DELAY)) {
        auto &stack = *stackOpt;

        // only remove state, if the stack would not be empty after pop
        if (stack->size() > 1) {
            stack->top()->suspendTasks();
            stack->pop();
            stack->top()->resumeTasks();
            std::cout << "Stack popped" << std::endl;
        } else {
            std::cout << "Stack not popped" << std::endl;
        }
    }
}

void StateMachine::pushStack(State *state) {
    if (auto stackOpt = stateStack.lock(portMAX_DELAY)) {
        auto &stack = *stackOpt;
        if (!stack->empty()) stack->top()->suspendTasks();
        stack->push(std::unique_ptr<State>{state});
        state->resumeTasks();
    }
}






