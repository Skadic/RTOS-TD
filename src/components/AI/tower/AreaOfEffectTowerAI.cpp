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
        for (auto &target : view) {
            if (towerData.getTargets().find(target) != towerData.getTargets().end()) {
                Health &health = view.get<Health>(target);
                health.value -= damage.value;
            }
            /*std::cout << "Has Pos: " << registry.has<Position>(target) << std::endl;
            std::cout << "Has Enemy: " << registry.has<Enemy>(target) << std::endl;
            std::cout << "Has Health: " << registry.has<Health>(target) << std::endl;
            std::cout << std::endl;*/
        }
        lastRun = high_resolution_clock::now();
    }
}


