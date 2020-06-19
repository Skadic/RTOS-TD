#pragma once

#include "Position.h"

struct Velocity {
    float dx;
    float dy;

    // Normalizes the vector, and returns its former length
    void normalize();

    // Points the Velocity towards a target with specified speed
    void towards(Position source, Position target, float speed);
};