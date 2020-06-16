#pragma once

extern "C" {
#include <FreeRTOS.h>
#include <semphr.h>
}

#include <optional>
#include <iostream>

template<class T>
class Guard;

template<class T>
class Mutex {
    SemaphoreHandle_t mutex;
    T value;

public:
    Mutex(T &&value, SemaphoreHandle_t m) : value{std::move(value)}, mutex{m} {}

    Mutex(const Mutex&) = delete;

    std::optional<Guard<T>> lock(TickType_t timeout = 10000) {
        if(xSemaphoreTake(mutex, timeout) == pdTRUE) {
            return std::make_optional<Guard<T>>(value, mutex);
        } else {
            return std::optional<Guard<T>>{};
        }
    };
};

template<class T>
class Guard {
    friend class Mutex<T>;
    T &value;
    SemaphoreHandle_t &mutex;
    bool movedFrom;


public:

    Guard(T &value, SemaphoreHandle_t &mutex) : value{value}, mutex{mutex}, movedFrom{false} {};
    Guard(const Guard&) = delete;
    Guard(Guard &&other) noexcept : value{other.value}, mutex{other.mutex}, movedFrom{false} {
        other.movedFrom = true;
    }

    ~Guard() {
        if(!movedFrom) {
            xSemaphoreGive(mutex);
        }
    };

    T &operator *() {
        return value;
    };

    T *operator ->() {
        return std::addressof(value);
    }
};