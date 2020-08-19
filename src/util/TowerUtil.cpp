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

    // Retrieve all Entities with position the enemy component and health (since these are the only entities)
    // that are eligible for being a tower's target
    auto view = registry.view<Position, Enemy, Health>();

    // Note, that even if there are enemies contained in the view,
    // there could still end up being no "closest target" if all entities are outside the tower's range
    const entt::entity *closestTarget = nullptr;
    double lowestDist = INFINITY;
    for (auto &target : towerData.getPotentialTargets()) {
        // Safety check in case an entity gets deleted while this function is running
        if(registry.valid(target)) {
            // If a more optimal target is found, update the variables accordingly
            Enemy &enemy = view.get<Enemy>(target);
            if (lowestDist > enemy.remainingDistance) {
                closestTarget = &target;
                lowestDist = enemy.remainingDistance;
            }
        }
    }

    // Return the closest target if there is one
    if(closestTarget) {
        return std::make_optional(*closestTarget);
    } else {
        return {};
    }
}