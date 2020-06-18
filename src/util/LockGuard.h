#pragma once

extern "C" {
#include <FreeRTOS.h>
#include <semphr.h>
}

#include <optional>
#include <iostream>
#include "Semaphore.h"

template<class T>
class Resource;


// Guards a resource with a Semaphore
template<class T>
class LockGuard {
    T value;
    Semaphore mutex;

public:
    LockGuard(T &&value, SemaphoreHandle_t m) : value{std::move(value)}, mutex{m} {}

    LockGuard(const LockGuard&) = delete;

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
    friend class LockGuard<T>;
    T &value;
    Semaphore &mutex;
    bool movedFrom;


public:

    Resource(T &value, Semaphore &mutex) : value{value}, mutex{mutex}, movedFrom{false} {};
    Resource(const Resource&) = delete;
    Resource(Resource &&other) noexcept : value{other.value}, mutex{other.mutex}, movedFrom{false} {
        other.movedFrom = true;
    }

    ~Resource() {
        if(!movedFrom) {
            mutex.unlock();
        }
    };

    T &operator *() {
        return value;
    };

    T *operator ->() {
        return std::addressof(value);
    }
};