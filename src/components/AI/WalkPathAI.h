#pragma once

#include "AI.h"
#include "../../states/State.h"
#include "../tilecomponents/TilePosition.h"
#include "../../states/gamestate/GameState.h"

/**
 * An AI that receives a path and walks along it.
 */
class WalkPathAI : public AI {
    /**
     * The path to walk along
     */
    std::vector<TilePosition> path;

    /**
     * The index of the tile the entity is walking towards next
     */
    int currentTile;

    /**
     * The remaining distance the entity needs to walk in order to reach the end of the path
     */
    double remainingDistance;
public:
    /**
     * Constructs a new WalkPathAI from a given path
     * @param entity The entity who is executing this AI
     * @param path The path to walk
     */
    explicit WalkPathAI(entt::entity &entity, std::vector<TilePosition> &path);

    void act(entt::registry &registry) override;
};
