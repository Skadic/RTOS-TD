#pragma once

#include "../AI.h"
#include "../../tilecomponents/Tower.h"
#include "../../Damage.h"
#include <chrono>

class AreaOfEffectTowerAI : public AI {
    int timeInterval;
    std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<double>> lastRun;

public:
    AreaOfEffectTowerAI(entt::entity self, int timeInterval);
    void act(entt::registry &registry) override;
};
