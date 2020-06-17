#pragma once

#include <vector>
#include <entity/registry.hpp>
#include "../util/Mutex.h"

extern "C" {
#include <FreeRTOS.h>
#include <task.h>
}

class State {

protected:
    Mutex<entt::registry> registry;
    std::vector<TaskHandle_t> tasks;

    State() : registry{entt::registry(), xSemaphoreCreateMutex()}, tasks{std::vector<TaskHandle_t>()} {}

public:
    ~State();
    void resumeTasks();
    void suspendTasks();
    Mutex<entt::registry> &getRegistry();
};