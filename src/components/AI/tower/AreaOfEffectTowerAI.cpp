//
// Created by skadic on 10.07.20.
//

#include <cmath>
#include "AreaOfEffectTowerAI.h"
#include "../../Health.h"
#include "../../Position.h"
#include "../../Enemy.h"

using namespace std::chrono;

AreaOfEffectTowerAI::AreaOfEffectTowerAI(entt::entity self, int timeInterval) :
    timeInterval{timeInterval},
    lastRun{high_resolution_clock::now()}{
    this->self = self;
}

void AreaOfEffectTowerAI::act(entt::registry &registry) {

    auto now = high_resolution_clock::now();

    auto time = duration_cast<std::chrono::milliseconds >(now - lastRun).count();

    if (time > timeInterval) {
        auto view = registry.view<Position, Enemy, Health>();
        Tower &towerData = registry.get<Tower>(this->self);
        Damage &damage = registry.get<Damage>(this->self);

        towerData.setActualTargets(towerData.getPotentialTargets());

        for (auto &target : towerData.getPotentialTargets()) {
            if(registry.valid(target)) {
                Health &health = view.get<Health>(target);
                health.value -= damage.value;
            }
        }

        towerData.getPotentialTargets().clear();
        lastRun = high_resolution_clock::now();
    }
}


