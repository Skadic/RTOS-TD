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

LaserTowerAI::LaserTowerAI(entt::entity self, int timeInterval, entt::registry &registry) :
    timeInterval{timeInterval},
    lastRun{std::chrono::high_resolution_clock::now()}{
    this->self = self;
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
            std::set<entt::entity> s {*closest};
            towerData.setActualTargets(s);
            lastRun = high_resolution_clock::now();
        }
    }
}
