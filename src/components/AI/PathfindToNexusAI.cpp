//
// Created by skadic on 24.06.20.
//

#include "PathfindToNexusAI.h"
#include "../TilePosition.h"
#include "AStar.h"
#include "../../util/GlobalConsts.h"
#include "../Velocity.h"
#include "../Hitbox.h"

PathfindToNexusAI::PathfindToNexusAI(GameState *state, entt::entity &entity, TilePosition nexusPos) : state{state}, nexusPos{nexusPos}, currentTile{0}, remainingDistance{-1} {
    this->self = entity;
}

void PathfindToNexusAI::act(entt::registry &registry) {

    if(path.empty() || currentTile < path.size()) {
        auto view = registry.view<Position, Velocity, AIComponent, Hitbox>();
        Position &pos = view.get<Position>(self);
        // If no path has been calculated yet, calculate it with the a star algorithm
        if (path.empty()) {
            path = aStarPathfinding(
                    TilePosition{static_cast<short>(pos.x / TILE_SIZE), static_cast<short>(pos.y / TILE_SIZE)},
                    nexusPos, state->getMap(), registry);
        }


        TilePosition &nextTile = path[currentTile];
        auto nextTilePos = Position{static_cast<float>(nextTile.x * TILE_SIZE),
                                    static_cast<float>(nextTile.y * TILE_SIZE)};

        Hitbox &hitbox = view.get<Hitbox>(self);

        nextTilePos.x += TILE_SIZE / 2 - hitbox.width / 2;
        nextTilePos.y += TILE_SIZE / 2 - hitbox.height / 2;

        while (pos.distance(nextTilePos) < 5) {
            currentTile++;
            nextTile = path[currentTile];
            nextTilePos = Position{static_cast<float>(nextTile.x * TILE_SIZE),
                                   static_cast<float>(nextTile.y * TILE_SIZE)};
        }

        this->remainingDistance = (path.size() - currentTile) * TILE_SIZE + pos.distance(nextTilePos);

        Velocity &vel = view.get<Velocity>(self);

        if(currentTile < path.size()) {
            vel.towards(pos, nextTilePos, ENEMY_SPEED);
        } else {
            vel.dx = 0;
            vel.dy = 0;
        }

    }

}
