#pragma once

#include "Position.h"

/**
 * A component representing the velocity of an entity
 */
struct Velocity {
    /**
     * The horizontal velocity
     */
    float dx;
    /**
     * The vertical velocity
     */
    float dy;

private:
    /**
     * The absolute max speed of the entity. This should not change
     */
    float maxSpeed;

    /**
     * The current max speed. This can be different from the absolute max speed,
     * if for example the entity has a temporary effect which decreases its speed.
     * The absolute max speed value can then be used to return the current max speed to its original value
     */
    float currentMaxSpeed;


public:

    /**
     * Creates a Velocity component which the specified velocity and maximum speed
     */
    Velocity(float dx, float dy, float maxSpeed);

    /**
     * Normalizes the vector
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

    /**
     * Sets the current max speed value to the absolute max speed value
     */
    void resetCurrentMaxSpeed();

    const float getMaxSpeed() const;

    float getCurrentMaxSpeed() const;

    void setCurrentMaxSpeed(float speed);

};