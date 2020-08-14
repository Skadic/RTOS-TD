#pragma once

#include <FreeRTOS.h>
#include <semphr.h>

/**
 * Abstraction around the FreeRTOS Semaphore
 * Can be locked and unlocked.
 */
class Semaphore {
    /**
     * The handle of the underlying semaphore
     */
    SemaphoreHandle_t semaphore;

public:
    /**
     * Creates a semaphore using a Semaphore handle created with the FreeRTOS emulator
     *
     * Example:
     * `Semaphore(xSemaphoreCreateMutex())` creates a Semaphore that works like a Mutex.
     *
     * @param sem A semaphore of an initialized semaphore
     */
    explicit Semaphore(SemaphoreHandle_t sem);

    /**
     * Upon destruction the semaphore needs to be deleted
     */
    ~Semaphore();

    /**
     * Tries to lock the Semaphore
     * @param timeout The time to wait for the Semaphore to become free
     * @return true, if locking was successful, false otherwise
     */
    bool lock(TickType_t timeout = 10000);

    /**
     * Unlocks this semaphore
     */
    void unlock();

    /**
     * Returns the underlying handle to the semaphore
     */
    SemaphoreHandle_t &getHandle();
};


