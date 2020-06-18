#pragma once

#include <FreeRTOS.h>
#include <semphr.h>

class Semaphore {
    SemaphoreHandle_t semaphore;

public:
    explicit Semaphore(SemaphoreHandle_t sem);
    ~Semaphore();

    bool lock(TickType_t timeout = 10000);
    void unlock();

    SemaphoreHandle_t &getHandle();
};


