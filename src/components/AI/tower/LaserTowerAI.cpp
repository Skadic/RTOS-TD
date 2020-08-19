//
// Created by skadic on 16.07.20.
//

#include <cmath>
#include "LaserTowerAI.h"
#include "../../Position.h"
#include "../../Enemy.h"
#include "../../Health.h"
#include "../../../util/TowerUtil.h"
#include <set>

using namespace std::chrono;

LaserTowerAI::LaserTowerAI(entt::entity self) : AI() {
    this->self = self;
}

void LaserTowerAI::act(entt::registry &registry) {
    auto view = registry.view<Position, Enemy, Health>();
    Tower &towerData = registry.get<Tower>(this->self);
    Damage &damage = registry.get<Damage>(this->self);

    // Calculate the enemy closest to the nexus, deal damage to it and set the actual target of this tower, to said enemy
    if (auto closest = closestTarget(towerData, registry)) {
        Health &health = view.get<Health>(*closest);
        health.value -= damage.value;
        std::set<entt::entity> s {*closest};
        towerData.setActualTargets(s);
    }

}
