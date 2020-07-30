//
// Created by skadic on 30.07.20.
//

#include <iostream>
#include "SimpleProjectileAI.h"
#include "../../Position.h"
#include "../../Velocity.h"
#include "../../Hitbox.h"
#include "../../tags/Delete.h"
#include "../../Health.h"
#include "../../Damage.h"

SimpleProjectileAI::SimpleProjectileAI(entt::entity self, entt::entity target, double speed) : target{target}, speed{speed}, lifetime{0} {
    this->self = self;
}

void SimpleProjectileAI::act(entt::registry &registry) {
    if(registry.valid(target)) {
        lifetime++;
        if(lifetime > 90) {
            registry.emplace_or_replace<Delete>(self);
        }

        Position &pos = registry.get<Position>(self);
        Velocity &vel = registry.get<Velocity>(self);
        Hitbox &hitbox = registry.get<Hitbox>(self);

        if(!registry.has<Position, Hitbox>(target)) return;

        Position &targetPos = registry.get<Position>(target);
        Hitbox &targetHitbox = registry.get<Hitbox>(target);

        vel.towards(pos, targetPos, speed);

        if (intersectHitbox(pos, hitbox, targetPos, targetHitbox)) {
            std::cout << "Target valid: " << registry.valid(target) << " Self valid: " << registry.valid(self)
                      << std::endl;
            Health &targetHealth = registry.get<Health>(target);
            Damage &damage = registry.get<Damage>(self);

            targetHealth.value -= damage.value;
            registry.emplace_or_replace<Delete>(self);
        }
    } else registry.emplace_or_replace<Delete>(self);
}
