//
// Created by skadic on 10.07.20.
//

#include "AreaOfEffectTowerAI.h"
#include "../../Health.h"
#include "../../Position.h"
#include "../../tags/Enemy.h"

using namespace std::chrono;

AreaOfEffectTowerAI::AreaOfEffectTowerAI(entt::entity self, int timeInterval, entt::registry &registry) :
    timeInterval{timeInterval},
    lastRun{std::chrono::high_resolution_clock::now()},
    towerData{registry.get<Tower>(self)},
    damage{registry.get<Damage>(self)}{
    this->self = self;
}

void AreaOfEffectTowerAI::act(entt::registry &registry) {

    auto now = high_resolution_clock::now();

    duration<double> time = duration_cast<duration<double>>(now - lastRun);

    if(time.count() * 1000 > timeInterval) {
        auto view = registry.view<Position, Enemy, Health>();
        Tower &towerData = registry.get<Tower>(this->self);
        for (auto &target : towerData.getTargets()) {
            if (registry.valid(target) && registry.has<Health>(target)) {
                Health &health = view.get<Health>(target);
                health.value -= damage.value;
            }
        }
        lastRun = high_resolution_clock::now();
    }
}


