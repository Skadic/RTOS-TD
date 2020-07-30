//
// Created by skadic on 30.07.20.
//

#include "ProjectileTowerAI.h"
#include "../../Damage.h"
#include "../../tilecomponents/TilePosition.h"
#include "../../../util/GlobalConsts.h"
#include "../../../util/spawn/EntitySpawn.h"
#include "../../Velocity.h"
#include "../../tilecomponents/Tower.h"
#include "../../../util/TowerUtil.h"
#include "../projectile/SimpleProjectileAI.h"
#include <chrono>

using namespace std::chrono;

ProjectileTowerAI::ProjectileTowerAI(entt::entity self, double speed) : AI(), projectileSpeed{speed}, lastRun{high_resolution_clock::now()} {
    this->self = self;
}

void ProjectileTowerAI::act(entt::registry &registry) {

    auto now = high_resolution_clock::now();
    auto duration = duration_cast<std::chrono::milliseconds>(now - lastRun).count();

    std::cout << duration << std::endl;

    Tower &towerData = registry.get<Tower>(self);
    if(towerData.hasPotentialTargets() && duration > 100) {
        TilePosition &tilePos =  registry.get<TilePosition>(self);
        Position pos =  tilePos.toPosition() + Position{ TILE_SIZE / 2, TILE_SIZE / 2} - Position{DEFAULT_PROJECTILE_SIZE / 2, DEFAULT_PROJECTILE_SIZE / 2};
        Damage &dmg = registry.get<Damage>(self);

        auto projectile = spawnProjectile(pos, registry);
        auto target = *closestTarget(towerData, registry);

        registry.emplace<Damage>(projectile, dmg.value);
        registry.emplace<AIComponent>(projectile, new SimpleProjectileAI(projectile, target, projectileSpeed));
        towerData.getPotentialTargets().clear();
        lastRun = high_resolution_clock::now();
    }
}
