#pragma once

extern "C" {
#include <TUM_Draw.h>
#include <TUM_Utils.h>
#include <TUM_Event.h>
#include <task.h>
#include <semphr.h>
}

#include <vector>
#include <entt.hpp>
#include "State.h"

class TestState : public State {
public:
    TestState();
};


std::vector<TaskHandle_t> initTestTasks(entt::registry &registry);

void renderTask(void *registryPointer);

void drawHitboxesTask(void *registryPointer);

void movementTask(void *registryPointer);

void bounceTask(void *registryPointer);

void spawnTask(void *registryPointer);