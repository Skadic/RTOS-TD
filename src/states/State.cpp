//

#include "State.h"
#include "../util/TaskUtils.h"
#include "../Game.h"
#include "../util/Log.h"
#include <iostream>

//
// Created by skadic on 29.05.20.
State::State() : registry{entt::registry(), xSemaphoreCreateMutex()}, tasks{std::vector<TaskHandle_t>()} {
    debug("State constructed");
}

State::~State() {
    deleteAll(this->tasks);
    debug("State deconstructed");
}

void State::resumeTasks() {
    resumeAll(this->tasks);
    debug("State tasks resumed");
}

void State::suspendTasks() {
    suspendAll(this->tasks);
    debug("State tasks suspended");
}

void State::addTask(TaskFunction_t task, const char *const pcName, uint16_t stackSize, UBaseType_t priority) {
    tasks.push_back(nullptr); // Make space for another task handle
    xTaskCreate(task, pcName, stackSize, this, priority, &tasks.back());
    vTaskSuspend(tasks.back());
    if(pcName != nullptr) debug("Created task " + std::string(pcName));
}

LockGuard<entt::registry> &State::getRegistry() {
    return this->registry;
}

Renderer &State::getRenderer() {
    return renderer;
}
