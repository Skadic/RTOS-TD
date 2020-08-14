#pragma once

#include "../AI.h"
#include "../../tilecomponents/Tower.h"
#include "../../Damage.h"
#include <chrono>

/**
 * A tower AI, that attacks all enemies within range at a time interval
 */
class AreaOfEffectTowerAI : public AI {

    /**
     * The type of time points
     */
    using time_point_t = std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<double>>;

    /**
     * The interval in ms at which the tower should attack
     */
    int timeInterval;

    /**
     * The time of the last firing operation
     */
    time_point_t lastRun;

public:
    /**
     * @param self The tower executing this AI
     * @param timeInterval The interval at which the tower should attack
     */
    AreaOfEffectTowerAI(entt::entity self, int timeInterval);
    void act(entt::registry &registry) override;
};
