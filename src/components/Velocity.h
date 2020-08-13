#pragma once

#include "Position.h"

struct Velocity {
    float dx;
    float dy;
private:
    float maxSpeed;
    float currentMaxSpeed;


public:

    Velocity(float dx, float dy, float maxSpeed);

    /**
     * Normalizes the vector, and returns its former length
     */
    void normalize();

    /**
     * Points the Velocity towards a target at max speed
     */
    void towards(Position source, Position target);

    /**
     * Points the Velocity towards a target with specified speed
     */
    void towards(Position source, Position target, float speed);

    void resetCurrentMaxSpeed();

    const float getMaxSpeed() const;

    float getCurrentMaxSpeed() const;

    void setCurrentMaxSpeed(float speed);

};