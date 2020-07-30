//
// Created by skadic on 30.07.20.
//

#include <cmath>
#include "TowerUtil.h"
#include "../components/Position.h"
#include "../components/Enemy.h"
#include "../components/Health.h"


std::optional<entt::entity> closestTarget(Tower &towerData, entt::registry &registry) {
    // If the tower has no potentialTargets, there is no target to return
    if(!towerData.hasPotentialTargets()) return {};

    auto view = registry.view<Position, Enemy, Health>();
    const entt::entity *closestTarget = nullptr;
    double lowestDist = INFINITY;
    for (auto &target : towerData.getPotentialTargets()) {
        if(registry.valid(target)) {
            Enemy &enemy = view.get<Enemy>(target);
            if (lowestDist > enemy.remainingDistance) {
                closestTarget = &target;
                lowestDist = enemy.remainingDistance;
            }
        }
    }

    if(closestTarget) {
        return std::make_optional(*closestTarget);
    } else {
        return {};
    }
}