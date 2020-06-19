//
// Created by skadic on 19.06.20.
//

#include "Log.h"
#include <iostream>
#include <FreeRTOS.h>
#include <task.h>


void logCurrentTaskName() {
    if(LOG_ENABLED) {
        std::cout << pcTaskGetName(xTaskGetCurrentTaskHandle()) << std::endl;
    }
}

void log(std::string msg) {
    if(LOG_ENABLED) {
        std::cout << msg << std::endl;
    }
}