#include <iostream>
#include "TaskUtils.h"

void suspendAll(const std::vector<TaskHandle_t> &tasks) {
    for(TaskHandle_t task : tasks) {
        vTaskSuspend(task);
    }
}

void resumeAll(const std::vector<TaskHandle_t> &tasks) {
    for(TaskHandle_t task : tasks) {
        vTaskResume(task);
    }
}

void deleteAll(const std::vector<TaskHandle_t> &tasks) {
    for(TaskHandle_t task : tasks) {
        vTaskDelete(task);
    }
}