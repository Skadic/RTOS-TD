#pragma once

#include "../AI.h"
#include "../../tilecomponents/Tower.h"
#include "../../Damage.h"
#include <chrono>

class LaserTowerAI : public AI {

public:
    LaserTowerAI(entt::entity self);
    void act(entt::registry &registry) override;
};