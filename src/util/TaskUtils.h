#pragma once

extern "C" {
#include <FreeRTOS.h>
#include <task.h>
}

#include <vector>

/**
 * Suspends all tasks in the given vector
 * @param tasks The tasks to suspend
 */
void suspendAll(const std::vector<TaskHandle_t> &tasks);

/**
 * Resumes all tasks in the given vector
 * @param tasks The tasks to resume
 */
void resumeAll(const std::vector<TaskHandle_t> &tasks);

/**
 * Deletes all tasks in the given vector
 * @param tasks The tasks to delete
 */
void deleteAll(const std::vector<TaskHandle_t> &tasks);
