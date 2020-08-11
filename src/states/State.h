#pragma once

#include <vector>
#include <entity/registry.hpp>
#include "../util/sync/LockGuard.h"
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

    State() : registry{entt::registry(), xSemaphoreCreateMutex()}, tasks{std::vector<TaskHandle_t>()} {
        std::cout << "Called" << std::endl;
    }

public:
    ~State();

    State(State&) = delete;

    void addTask(TaskFunction_t task, const char *pcName, uint16_t stackSize, UBaseType_t priority);
    void resumeTasks();
    void suspendTasks();
    LockGuard<entt::registry> &getRegistry();
    Renderer &getRenderer();
};