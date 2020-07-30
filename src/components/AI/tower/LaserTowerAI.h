#pragma once

#include "../AI.h"
#include "../../tilecomponents/Tower.h"
#include "../../Damage.h"
#include <chrono>

class LaserTowerAI : public AI {
    int timeInterval;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastRun;

public:
    LaserTowerAI(entt::entity self, int timeInterval);
    void act(entt::registry &registry) override;
};