//
// Created by skadic on 18.06.20.
//

#include "Semaphore.h"

Semaphore::Semaphore(SemaphoreHandle_t sem) : semaphore{sem} {}

Semaphore::~Semaphore() {
    vSemaphoreDelete(this->semaphore);
}

bool Semaphore::lock(TickType_t timeout) {
    return xSemaphoreTake(this->semaphore, timeout);
}

void Semaphore::unlock() {
    xSemaphoreGive(this->semaphore);
}

SemaphoreHandle_t &Semaphore::getHandle() {
    return this->semaphore;
}
