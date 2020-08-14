#pragma once

extern "C" {
#include <FreeRTOS.h>
#include <semphr.h>
}

#include <optional>
#include <iostream>
#include "Semaphore.h"


/**
 * Represents a value successfully retrieved from a LockGuard<T>
 * @tparam T The type of the underlying value
 */
template<class T>
class Resource;


/**
 * Guards a value using a semaphore. Using the lock method, this value can be retrieved, wrapped in a Resource object.
 * The purpose of this class is to control whether a thread can access the underlying value and
 * thus protect it from race conditions etc.
 *
 * Whether a thread can lock this guard, depends on the Semaphore chosen.
 * E.g. a mutex would only allow 1 thread at a time to lock this LockGuard.
 *
 * @tparam T The type of the guarded value
 */
template<class T>
class LockGuard {
    /**
     * The value to be guarded
     */
    T value;

    /**
     * The semaphore that guards this value
     */
    Semaphore mutex;

public:
    /**
     * Creates a new LockGuard from a value and a semaphore handle
     * @param value The value to be guarded
     * @param mutex A Semaphore handle created by the FreeRTOS emulator. This will be used internally to lock the resource.
     */
    LockGuard(T &&value, SemaphoreHandle_t mutex) : value{std::move(value)}, mutex{mutex} {}

    /**
     * A LockGuard cannot be copied, as that would defeat its purpose
     */
    LockGuard(const LockGuard&) = delete;

    /**
     * Tries locking the internal semaphore and if successful, returns the guarded value
     * @param timeout The time to wait for the resource to become free to lock.
     * @return The value wrapped in a Resource object, which is again wrapped in an optional if the locking succeeds.
     * Otherwise returns an empty optional.
     *
     * @relates Resource
     */
    std::optional<Resource<T>> lock(TickType_t timeout = 10000) {
        if(mutex.lock(timeout)) {
            return std::make_optional<Resource<T>>(value, mutex);
        } else {
            return std::optional<Resource<T>>{};
        }
    };
};

template<class T>
class Resource {
    /**
     * Only the LockGuard can access the internals of this class
     */
    friend class LockGuard<T>;

    /**
     * A reference to the guarded value
     */
    T &value;

    /**
     * The mutex used to lock the value
     */
    Semaphore &mutex;

    /**
     * Whether the value has been moved from this Resource object.
     * If this is the case, the mutex may not be unlocked when this Resource object is deconstructed.
     * Because the value has been moved elsewhere, it is not safe for another thead to access it.
     * For that to be safe, the Resource holding the value must be deconstructed.
     */
    bool movedFrom;


public:
    /**
     * Creates a Resource object from a value and a mutex. These are received from the LockGuard.
     * Because this Resource is just being created, the value has not been moved from it. So movedFrom is false.
     * @param value The value to hold
     * @param mutex The mutex that guards the value
     */
    Resource(T &value, Semaphore &mutex) : value{value}, mutex{mutex}, movedFrom{false} {};

    /**
     * Resources cannot be copied, as again, this would defeat their purpose.
     */
    Resource(const Resource&) = delete;

    /**
     * Allow for the assignment of rvalues to a new Resource. For example in the case
     * `auto stateOpLock = *stateOperation.lock(portMAX_DELAY);` (@see Game) where `stateOperation` is a LockGuard
     * Because the copy constructor is deleted, we need this move constructor for this to be possible.
     *
     * This constructor moves the value to this one and sets the `movedFrom` member of the other Resource to `true`,
     * because we moved the other Resource's value to this one
     *
     * @param other The Resource to be moved from
     */
    Resource(Resource &&other) noexcept : value{other.value}, mutex{other.mutex}, movedFrom{false} {
        other.movedFrom = true;
    }


    /**
     * On deconstruction we check whether this Resource's value been moved to another Resource object.
     * If that is the case, then we may not unlock the mutex, as there is still another Resource object with a reference
     * to the value previously held by this Resource object.
     * Otherwise we unlock the mutex, as we can be sure that this Resource object is the only one that holds a reference
     * to the value.
     * This means that once a Resource object goes out of scope and is destroyed, the value will be automatically unlocked,
     * if there is no Resource object, which the value has been moved to.
     */
    ~Resource() {
        if(!movedFrom) {
            mutex.unlock();
        }
    };

    /**
     * Sets the value
     * @param val The new value
     */
    void set(T val) {
        value = val;
    }

    /**
     * Allows dereferencing the Resource object to expose the underlying value
     * @return The value held by the Resource
     */
    T &operator *() {
        return value;
    };

    /**
     * Allows to access the underlying value and execute methods on it
     */
    T *operator ->() {
        return std::addressof(value);
    }
};