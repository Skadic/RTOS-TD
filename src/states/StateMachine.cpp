//
// Created by skadic on 16.06.20.
//

#include "StateMachine.h"

#include "State.h"
#include "../Game.h"
#include "../util/Log.h"
#include <iostream>

StateMachine::StateMachine() : stateStack{std::stack<std::unique_ptr<State>>(), xSemaphoreCreateMutex()} {}

State &StateMachine::activeState() {
    return *(*stateStack.lock(portMAX_DELAY))->top();
}

void StateMachine::popStack2X() {

    if (auto stackOpt = stateStack.lock(portMAX_DELAY)) {
        auto &stack = *stackOpt;

        if (stack->size() >= 2) {
            stack->top()->suspendTasks();
            stack->pop();
            stack->pop();
            debug("2 states popped off stack");
            if (!stack->empty()) {
                stack->top()->resumeTasks();
            } else {
                debug("Last stack element popped");
            }
        } else {
            warn("2X pop not executed because stack has less than 2 elements");
        }
    }
}

void StateMachine::popStack() {

    if (auto stackOpt = stateStack.lock(portMAX_DELAY)) {
        auto &stack = *stackOpt;

        // only remove state, if the stack would not be empty after pop
        if (!stack->empty()) {
            stack->top()->suspendTasks();
            stack->pop();
            debug("State popped off stack");
            if (!stack->empty()) {
                stack->top()->resumeTasks();
            } else {
                debug("Last stack element popped");
            }
        } else {
            warn("Pop not executed because stack is empty");
        }
    }
}

bool StateMachine::empty() {
    return (**stateStack.lock()).empty();
}

void StateMachine::pushStack(State* state) {
    if (auto stackOpt = stateStack.lock(portMAX_DELAY)) {
        auto &stack = *stackOpt;
        if (!stack->empty()) stack->top()->suspendTasks();
        stack->push(std::unique_ptr<State>(state));
        stack->top()->resumeTasks();
        Game::get().getSwapBufferSignal().unlock();
    }
}






