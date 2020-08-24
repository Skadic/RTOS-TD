//
// Created by skadic on 19.06.20.
//

#include "Log.h"
#include <iostream>
#include <utility>
#include <FreeRTOS.h>
#include <task.h>
#include <thread>

std::string levelName(LogLevel level) {
    switch (level) {
        case OFF: return "OFF";
        case DEBUG: return "DEBUG";
        case INFO: return "INFO";
        case WARN: return "WARN";
        case ERROR: return "ERROR";
        case FATAL: return "FATAL";
    }
}


/**
 * Logs a message at the specified logging level
 * @param msg The message to send
 * @param level The logging level
 */
void log(std::string msg, LogLevel level) {
    if(LOG_LEVEL != OFF && LOG_LEVEL >= level) {
        std::cout << "[thread-"<< std::this_thread::get_id() << "] " << levelName(level) << ": " << msg << std::endl;
    }
}

void debug(std::string msg) {
    log(std::move(msg), DEBUG);
}

void info(std::string msg) {
    log(std::move(msg), INFO);
}

void warn(std::string msg) {
    log(std::move(msg), WARN);
}

void error(std::string msg) {
    log(std::move(msg), ERROR);
}

void fatal(std::string msg) {
    log(std::move(msg), FATAL);
}
