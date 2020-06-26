#pragma once

#include "AI.h"
#include "../../states/State.h"
#include "../TilePosition.h"
#include "../../states/GameState.h"

class PathfindToNexusAI : public AI {
    GameState *state;
    std::vector<TilePosition> path;
    int currentTile;
    TilePosition nexusPos;
    double remainingDistance;
public:
    explicit PathfindToNexusAI(GameState *state, entt::entity &, TilePosition nexusPos);
    void act(entt::registry &registry) override;
};
