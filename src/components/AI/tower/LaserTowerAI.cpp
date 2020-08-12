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

    if (auto closest = closestTarget(towerData, registry)) {
        Health &health = view.get<Health>(*closest);
        health.value -= damage.value;
        std::set<entt::entity> s {*closest};
        towerData.setActualTargets(s);
    }

}
