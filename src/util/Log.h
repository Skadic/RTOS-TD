#pragma once

#include <string>

/**
 * Whether logging is enabled
 */
inline const auto LOG_ENABLED = false;

/**
 * Logs the name of the task which this function is called in
 */
void logCurrentTaskName();

/**
 * Logs a message
 * @param msg The message to send
 */
void log(std::string msg);