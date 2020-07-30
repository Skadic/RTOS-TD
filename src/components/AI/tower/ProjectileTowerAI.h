#pragma once

#include <chrono>
#include "../AI.h"

class ProjectileTowerAI : public AI {
    double projectileSpeed;
    std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<double>> lastRun;
public:
    ProjectileTowerAI(entt::entity self, double speed);
    void act(entt::registry &registry) override;
};
