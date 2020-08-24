#pragma once

#include <string>

enum LogLevel {
    OFF,
    FATAL,
    ERROR,
    WARN,
    INFO,
    DEBUG
};


/**
 * Which level of logging is enabled
 */
inline const LogLevel LOG_LEVEL = INFO;

/**
 * Logs the name of the task which this function is called in
 */
void logCurrentTaskName();

/**
 * Logs a message at debug logging level
 * @param msg The message to send
 */
void debug(std::string msg);

/**
 * Logs a message at info logging level
 * @param msg The message to send
 */
void info(std::string msg);

/**
 * Logs a message at warn logging level
 * @param msg The message to send
 */
void warn(std::string msg);

/**
 * Logs a message at error logging level
 * @param msg The message to send
 */
void error(std::string msg);

/**
 * Logs a message at fatal logging level
 * @param msg The message to send
 */
void fatal(std::string msg);