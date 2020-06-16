#pragma once

extern "C" {
#include <FreeRTOS.h>
#include <task.h>
}

#include <vector>

void suspendAll(const std::vector<TaskHandle_t> &tasks);

void resumeAll(const std::vector<TaskHandle_t> &tasks);

void deleteAll(const std::vector<TaskHandle_t> &tasks);
