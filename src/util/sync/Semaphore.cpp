//
// Created by skadic on 18.06.20.
//

#include "Semaphore.h"

Semaphore::Semaphore(SemaphoreHandle_t sem) : semaphore{sem} {}

Semaphore::~Semaphore() {
    // Deletes the semaphore upon deconstruction of the object
    vSemaphoreDelete(this->semaphore);
}

bool Semaphore::lock(TickType_t timeout) {
    // Tries to take the semaphore.
    return xSemaphoreTake(this->semaphore, timeout);
}

void Semaphore::unlock() {
    // Releases the semaphore.
    xSemaphoreGive(this->semaphore);
}

SemaphoreHandle_t &Semaphore::getHandle() {
    return this->semaphore;
}
