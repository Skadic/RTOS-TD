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
#include "../projectile/TargetedSimpleProjectileAI.h"
#include <chrono>

using namespace std::chrono;

ProjectileTowerAI::ProjectileTowerAI(entt::entity self, double projectileSpeed, double firingInterval) : AI(),
    projectileSpeed{projectileSpeed}, firingInterval{firingInterval}, lastRun{high_resolution_clock::now()} {
    this->self = self;
}

void ProjectileTowerAI::act(entt::registry &registry) {

    // Check how much time has elapsed since the last time the tower fired
    auto now = high_resolution_clock::now();
    auto duration = duration_cast<std::chrono::milliseconds>(now - lastRun).count();

    Tower &towerData = registry.get<Tower>(self);
    // Only continue if the tower has potential targets and enough time has passed since the last run
    if(towerData.hasPotentialTargets() && duration > firingInterval) {
        TilePosition &tilePos =  registry.get<TilePosition>(self);
        Position pos =  tilePos.toPosition() + Position{ TILE_SIZE / 2, TILE_SIZE / 2} - Position{DEFAULT_PROJECTILE_SIZE / 2, DEFAULT_PROJECTILE_SIZE / 2};
        Damage &dmg = registry.get<Damage>(self);

        // Spawn a projectile and determine the target which is the closest to the goal
        auto projectile = spawnProjectile(pos, registry);
        auto target = *closestTarget(towerData, registry);

        // Give the projectile a damage value and its AI
        registry.emplace<Damage>(projectile, dmg.value);
        registry.emplace<AIComponent>(projectile, new TargetedSimpleProjectileAI(projectile, target, projectileSpeed));

        towerData.getPotentialTargets().clear();
        lastRun = high_resolution_clock::now();
    }
}
