#pragma once

#include <vector>

extern "C" {
#include <FreeRTOS.h>
#include <task.h>
}

class State {

protected:
    std::vector<TaskHandle_t> tasks;

public:

    ~State();
    void resumeTasks();
    void suspendTasks();
};