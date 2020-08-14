#pragma once

#include "../AI.h"
#include "../../tilecomponents/Tower.h"
#include "../../Damage.h"
#include <chrono>

/**
 * A tower AI which attacks the enemy in range that is closest to the nexus with a continuously firing laser beam
 */
class LaserTowerAI : public AI {

public:
    explicit LaserTowerAI(entt::entity self);
    void act(entt::registry &registry) override;
};