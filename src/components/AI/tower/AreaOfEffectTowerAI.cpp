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

    // Calculate the time since the last time the tower attacked
    auto now = high_resolution_clock::now();
    auto time = duration_cast<std::chrono::milliseconds >(now - lastRun).count();

    // If the set time interval expired, attack again
    if (time > timeInterval) {
        auto view = registry.view<Position, Enemy, Health>();
        Tower &towerData = registry.get<Tower>(this->self);
        Damage &damage = registry.get<Damage>(this->self);

        // The tower should attack all targets in range. So all potential targets (= enemies in range) are also
        // its actual targets
        towerData.setActualTargets(towerData.getPotentialTargets());

        // Deal damage to all targets
        for (auto &target : towerData.getPotentialTargets()) {
            if(registry.valid(target)) {
                Health &health = view.get<Health>(target);
                health.value -= damage.value;
            }
        }

        // Clear the potential targets so that they are not accidentally reused
        // This could be a problem if the task that refreshes the data CollisionTable does not run between two tower attacks
        // and if in addition at least one of the tower's targets was deleted in the meanwhile.
        // The deleted entity would not be valid anymore and would cause an error
        towerData.getPotentialTargets().clear();

        // Reset the last run time
        lastRun = high_resolution_clock::now();
    }
}


