#pragma once

#include "../AI.h"
#include "../../tilecomponents/Tower.h"
#include "../../Damage.h"
#include <chrono>

class AreaOfEffectTowerAI : public AI {
    int timeInterval;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastRun;

public:
    AreaOfEffectTowerAI(entt::entity self, int timeInterval, entt::registry &registry);
    void act(entt::registry &registry) override;
};
