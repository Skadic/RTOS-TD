#pragma once

#include <vector>
#include <entity/registry.hpp>
#include "../util/LockGuard.h"
#include "../util/Renderer.h"

extern "C" {
#include <FreeRTOS.h>
#include <task.h>
}

class State {

protected:
    LockGuard<entt::registry> registry;
    std::vector<TaskHandle_t> tasks;
    Renderer renderer;

    State() : registry{entt::registry(), xSemaphoreCreateMutex()}, tasks{std::vector<TaskHandle_t>()} {}

public:
    ~State();
    void resumeTasks();
    void suspendTasks();
    LockGuard<entt::registry> &getRegistry();
    Renderer &getRenderer();
};