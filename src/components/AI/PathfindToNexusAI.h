#pragma once

#include "AI.h"
#include "../../states/State.h"
#include "../tilecomponents/TilePosition.h"
#include "../../states/gamestate/GameState.h"

class PathfindToNexusAI : public AI {
    GameState *state;
    std::vector<TilePosition> path;
    int currentTile;
    double remainingDistance;
public:
    explicit PathfindToNexusAI(GameState *state, entt::entity &entity, std::vector<TilePosition> &path);
    void act(entt::registry &registry) override;
};
