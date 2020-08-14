#pragma once

#include <chrono>
#include "../AI.h"

/**
 * A tower AI that shoots projectiles at the enemy in range that is closest to the nexus
 */
class ProjectileTowerAI : public AI {

    /**
     * The type of time points
     */
    using time_point_t = std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<double>>;

    /**
     * The speed of fired projectiles
     */
    double projectileSpeed;

    /**
     * The interval in ms at which the tower should attack
     */
    double firingInterval;

    /**
     * The time of the last firing operation
     */
    time_point_t lastRun;
public:
    /**
     * Constructs a new AI object with a projectile speed and a firing interval
     * @param self The tower executing the AI
     * @param projectileSpeed The speed of the fired projectiles
     * @param firingInterval The interval at which the tower should attack
     */
    ProjectileTowerAI(entt::entity self, double projectileSpeed, double firingInterval);
    void act(entt::registry &registry) override;
};
