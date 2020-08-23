#pragma once

#include <vector>
#include <entity/registry.hpp>
#include "../util/sync/LockGuard.h"
#include "../util/Renderer.h"

extern "C" {
#include <FreeRTOS.h>
#include <task.h>
}

/**
 * A general State class
 * A state represents a "situation" the game can be in
 * This can be a menu, or in game for example
 */
class State {

protected:
    /**
     * Each State has its own registry with its own entities protected behind a LockGuard
     */
    LockGuard<entt::registry> registry;

    /**
     * All the tasks that the state needs to run
     */
    std::vector<TaskHandle_t> tasks;

    /**
     * A renderer for this state
     */
    Renderer renderer;

    /**
     * Constructs a state with a normal mutex to protect the registry
     */
    State() : registry{entt::registry(), xSemaphoreCreateMutex()}, tasks{std::vector<TaskHandle_t>()} {}

public:

    /**
     * On deletion all of this state's tasks must be deleted
     */
    virtual ~State();

    /**
     * States cannot be copied
     */
    State(State&) = delete;

    /**
     * Adds a task to the tasks vector. The argument given to the task is a pointer to the state itself
     * @param task The task function the task is supposed to run
     * @param pcName A name for the task. For debugging purposes only
     * @param stackSize A stack size for this task
     * @param priority The priority of this task
     */
    void addTask(TaskFunction_t task, const char *pcName, uint16_t stackSize, UBaseType_t priority);

    /**
     * Resumes all this state's tasks
     */
    void resumeTasks();

    /**
     * Suspends all of this state's tasks
     */
    void suspendTasks();

    /**
     * Gets the registry guarded by a LockGuard
     */
    LockGuard<entt::registry> &getRegistry();

    Renderer &getRenderer();
};