//
// Created by skadic on 29.05.20.
//

#include "State.h"
#include "../util/TaskUtils.h"
#include "../Game.h"
#include <iostream>

void State::resumeTasks() {
    resumeAll(this->tasks);
}

void State::suspendTasks() {
    suspendAll(this->tasks);
}

State::~State() {
    deleteAll(this->tasks);
    std::cout << "State deconstructed" << std::endl;
}

void State::addTask(TaskFunction_t task, const char *const pcName, uint16_t stackSize, UBaseType_t priority) {
    tasks.push_back(nullptr); // Make space for another task handle
    xTaskCreate(task, pcName, stackSize, this, priority, &tasks.back());
    vTaskSuspend(tasks.back());
}

LockGuard<entt::registry> &State::getRegistry() {
    return this->registry;
}

Renderer &State::getRenderer() {
    return renderer;
}
