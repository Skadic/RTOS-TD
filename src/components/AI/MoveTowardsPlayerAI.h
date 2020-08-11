#pragma once

#include "AI.h"
#include "../../states/gamestate/GameState.h"

class MoveTowardsPlayerAI : public AI {
    State *state;
public:
    explicit MoveTowardsPlayerAI(State *state, entt::entity &entity);
    void act(entt::registry &registry) override;
};
