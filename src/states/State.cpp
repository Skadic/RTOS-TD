//
// Created by skadic on 29.05.20.
//

#include "State.h"
#include "../util/TaskUtils.h"
#include <iostream>

void State::resumeTasks() {
    resumeAll(this->tasks);
}

void State::suspendTasks() {
    suspendAll(this->tasks);
}

State::~State() {
    deleteAll(this->tasks);
}