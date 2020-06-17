//
// Created by skadic on 16.06.20.
//

#include "StateMachine.h"

#include "State.h"
#include <iostream>

StateMachine::StateMachine() {
    queueLock = xSemaphoreCreateMutex();
    nextStateQueue = xQueueCreate(3, sizeof(State*));
    operationQueue = xQueueCreate(3, sizeof(StateChange));
    changed = false;
}

StateChange StateMachine::handleOperation() {
    if(changed && xSemaphoreTake(queueLock, portMAX_DELAY) == pdTRUE){
        State* nextState = nullptr;
        StateChange change = NONE;

        // If there is an operation to be handled
        if(xQueueReceive(operationQueue, &change, portMAX_DELAY) == pdTRUE) {
            switch (change) {
                case POP: {
                    stateStack.top()->suspendTasks();
                    stateStack.pop();
                    stateStack.top()->resumeTasks();
                };
                case PUSH: {
                    // If we are supposed to execute a push operation, we need to retrieve the state to push from the message queue
                    if (xQueueReceive(nextStateQueue, &nextState, 0) == pdTRUE) {
                        if(!stateStack.empty()) stateStack.top()->suspendTasks();
                        stateStack.push(std::unique_ptr<State>{nextState});
                        nextState->resumeTasks();
                    } else {
                        std::cout << "Push operation requested, but no state to push was given" << std::endl;
                        change = NONE;
                    }
                }
                case NONE : {}
            }

        }
        this->changed = false;
        xSemaphoreGive(queueLock);
        return change;
    }
    return NONE;
}

void StateMachine::enqueuePush(State *state) {
    static const StateChange push = PUSH;

    if(xSemaphoreTake(queueLock, portMAX_DELAY) == pdTRUE){
        xQueueSend(nextStateQueue, &state, portMAX_DELAY);
        xQueueSend(operationQueue, &push, portMAX_DELAY);
        changed = true;
        xSemaphoreGive(queueLock);
    }
}

void StateMachine::enqueuePop() {
    static const StateChange pop = POP;

    if(xSemaphoreTake(queueLock, portMAX_DELAY) == pdTRUE){
        xQueueSend(operationQueue, &pop, portMAX_DELAY);
        changed = true;
        xSemaphoreGive(queueLock);
    }
}

bool StateMachine::hasChanged() {
    return this->changed;
}

State &StateMachine::activeState() {
    return *this->stateStack.top();
}






