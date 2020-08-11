//
// Created by skadic on 16.06.20.
//

#include "StateMachine.h"

#include "State.h"
#include "../Game.h"
#include <iostream>

StateMachine::StateMachine() : stateStack{std::stack<std::unique_ptr<State>>(), xSemaphoreCreateMutex()} {}

State &StateMachine::activeState() {
    return *(*stateStack.lock(portMAX_DELAY))->top();
}

void StateMachine::popStack2X() {

    if (auto stackOpt = stateStack.lock(portMAX_DELAY)) {
        auto &stack = *stackOpt;

        if (stack->size() != 2) {
            stack->top()->suspendTasks();
            stack->pop();
            stack->pop();
            stack->top()->resumeTasks();
            std::cout << "Stack popped" << std::endl;
        } else if (stack->size() == 2){
            stack->pop();
            std::cout << "Last stack element popped" << std::endl;
        }
    }
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
        } else if (stack->size() == 1){
            stack->pop();
            std::cout << "Last stack element popped" << std::endl;
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






