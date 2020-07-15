#pragma once

struct Enemy {
    /**
     * The remaining distance the enemy has to walk to the goal
     */
    double remainingDistance;

    Enemy() : remainingDistance{-1} {}
};