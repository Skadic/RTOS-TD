#pragma once

/**
 * A component representing an enemy and relevant data
 */
struct Enemy {
    /**
     * The remaining distance the enemy has to walk to the goal
     */
    double remainingDistance;

    /**
     * Constructs a new Enemy component. The remaining distance is initially set to -1
     */
    Enemy() : remainingDistance{-1} {}
};