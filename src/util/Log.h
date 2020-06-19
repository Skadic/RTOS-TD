#pragma once

#include <string>

inline const auto LOG_ENABLED = false;

void logCurrentTaskName();

void log(std::string msg);