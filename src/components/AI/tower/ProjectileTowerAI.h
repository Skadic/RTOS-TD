#pragma once

#include <chrono>
#include "../AI.h"

class ProjectileTowerAI : public AI {
    double projectileSpeed;
    double firingInterval;

    std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<double>> lastRun;
public:
    ProjectileTowerAI(entt::entity self, double speed, double firingInterval);
    void act(entt::registry &registry) override;
};
