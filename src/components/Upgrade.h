#pragma once

#include <functional>
#include <utility>
#include "Damage.h"

/**
 * A component that contains a tower's level and a function to scale its damage with
 */
struct Upgrade
{
    /**
     * The tower's level
     */
    int level;

    /**
     * The factor by which the damage should be scaled each level
     */
    float factor;

    /**
     * Create a new Upgrade object to scale the tower's damage
     */
    explicit Upgrade(float factor) : level{0}, factor{factor} {}

    /**
     * Increases the tower's level by one
     */
    void levelUp();

    /**
     * Modifies the damage component to contain the scaled damage value according to the tower's level
     * @param damage The damage component to modify
     */
    void scaleDamage(Damage &damage);

};
