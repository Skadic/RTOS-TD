//
// Created by skadic on 30.07.20.
//

#include <iostream>
#include "TargetedSimpleProjectileAI.h"
#include "../../Position.h"
#include "../../Velocity.h"
#include "../../Hitbox.h"
#include "../../tags/Delete.h"
#include "../../Health.h"
#include "../../Damage.h"

TargetedSimpleProjectileAI::TargetedSimpleProjectileAI(entt::entity self, entt::entity target, double speed) : target{target}, speed{speed}, lifetime{0} {
    this->self = self;
}

void TargetedSimpleProjectileAI::act(entt::registry &registry) {
    // Safety check, if for example the target is killed by another tower etc.
    // If that is the case, the projectile should be deleted, as it does not have a target anymore
    if(registry.valid(target)) {

        // This is used to restrict the allowed number of ticks that a projectile can live. If those expire, the
        // projectile should be deleted
        lifetime++;
        if(lifetime > 90) {
            registry.emplace_or_replace<Delete>(self);
        }

        Position &pos = registry.get<Position>(self);
        Velocity &vel = registry.get<Velocity>(self);
        Hitbox &hitbox = registry.get<Hitbox>(self);

        // Safety check
        if(!registry.has<Position, Hitbox>(target)) return;

        Position &targetPos = registry.get<Position>(target);
        Hitbox &targetHitbox = registry.get<Hitbox>(target);

        // Point the projectile's movement vector towards the target
        vel.towards(pos, targetPos, speed);

        // If the projectile collides with its target, it should deal its damage and be deleted right after
        if (intersectHitbox(pos, hitbox, targetPos, targetHitbox)) {
            Health &targetHealth = registry.get<Health>(target);
            Damage &damage = registry.get<Damage>(self);

            targetHealth.value -= damage.value;
            registry.emplace_or_replace<Delete>(self);
        }
    } else registry.emplace_or_replace<Delete>(self);
}
