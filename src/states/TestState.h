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

void renderTask(void *statePointer);

void drawHitboxesTask(void *statePointer);

void movementTask(void *statePointer);

void bounceTask(void *statePointer);

void spawnTask(void *statePointer);