//
// Created by skadic on 19.06.20.
//

#include "MoveTowardsPlayerAI.h"
#include "../tags/Player.h"
#include "../Position.h"
#include "../Velocity.h"
#include "../../util/GlobalConsts.h"
#include "../../util/Log.h"

MoveTowardsPlayerAI::MoveTowardsPlayerAI(State *state, entt::entity &entity) : state{state}{
    this->self = entity;
}

void MoveTowardsPlayerAI::act(entt::registry &registry) {
    auto playerView = registry.view<Player, Position>();
    auto selfView = registry.view<Position, Velocity, AIComponent>();
    log("AI acting");
    for(auto &player : playerView) {
        Velocity &vel = selfView.get<Velocity>(this->self);
        Position &pos = selfView.get<Position>(this->self);
        Position &playerPos = playerView.get<Position>(player);

        vel.towards(pos, playerPos, ENEMY_SPEED);
    }
}
