#pragma once

struct Velocity {
    float dx;
    float dy;

    // Normalizes the vector, and returns its former length
    void normalize();
};