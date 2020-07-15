//
// Created by skadic on 16.07.20.
//

#include <cmath>
#include "LaserTowerAI.h"
#include "../../Position.h"
#include "../../Enemy.h"
#include "../../Health.h"

using namespace std::chrono;

LaserTowerAI::LaserTowerAI(entt::entity self, int timeInterval, entt::registry &registry) :
    timeInterval{timeInterval},
    lastRun{std::chrono::high_resolution_clock::now()}{
    this->self = self;
}

std::optional<entt::entity> closestTarget(Tower &towerData, entt::registry &registry) {
    // If the tower has no targets, there is no target to return
    if(!towerData.hasTargets()) return {};

    auto view = registry.view<Position, Enemy, Health>();
    const entt::entity *closestTarget = nullptr;
    double lowestDist = INFINITY;
    for (auto &target : towerData.getTargets()) {
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

void LaserTowerAI::act(entt::registry &registry) {

    auto now = high_resolution_clock::now();

    duration<double> time = duration_cast<duration<double>>(now - lastRun);

    if (time.count() * 1000 > timeInterval) {
        auto view = registry.view<Position, Enemy, Health>();
        Tower &towerData = registry.get<Tower>(this->self);
        Damage &damage = registry.get<Damage>(this->self);

        if (auto closest = closestTarget(towerData, registry)) {
            Health &health = view.get<Health>(*closest);
            health.value -= damage.value;
            lastRun = high_resolution_clock::now();
        }
    }
}
