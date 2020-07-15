//
// Created by skadic on 24.06.20.
//

#include "PathfindToNexusAI.h"
#include "../tilecomponents/TilePosition.h"
#include "AStar.h"
#include "../../util/GlobalConsts.h"
#include "../Velocity.h"
#include "../Hitbox.h"
#include "../Enemy.h"

PathfindToNexusAI::PathfindToNexusAI(GameState *state, entt::entity &entity, std::vector<TilePosition> &path) : state{state}, currentTile{0}, path{path}, remainingDistance{-1} {
    this->self = entity;
}

void PathfindToNexusAI::act(entt::registry &registry) {

    if(!path.empty() && currentTile < path.size()) {
        auto view = registry.view<Position, Velocity, AIComponent, Hitbox>();
        Position &pos = view.get<Position>(self);

        // The next tile the enemy should reach
        TilePosition &nextTile = path[currentTile];
        auto nextTilePos = Position{static_cast<float>(nextTile.x * TILE_SIZE),
                                    static_cast<float>(nextTile.y * TILE_SIZE)};

        // Modify the next tile position, so that the enemy aims for the center of the tile
        Hitbox &hitbox = view.get<Hitbox>(self);
        nextTilePos.x += TILE_SIZE / 2 - hitbox.width / 2;
        nextTilePos.y += TILE_SIZE / 2 - hitbox.height / 2;


        // If the enemy has reached the next tile, set the next tile
        while (pos.distance(nextTilePos) < 5) {
            currentTile++;
            nextTile = path[currentTile];
            nextTilePos = Position{static_cast<float>(nextTile.x * TILE_SIZE),
                                   static_cast<float>(nextTile.y * TILE_SIZE)};
        }

        // Calculate the remaining distance to the goal
        Enemy &enemy = registry.get<Enemy>(self);
        enemy.remainingDistance = (path.size() - currentTile) * TILE_SIZE + pos.distance(nextTilePos);

        // Point the Movement direction to the next tile
        Velocity &vel = view.get<Velocity>(self);

        if(currentTile < path.size()) {
            vel.towards(pos, nextTilePos, ENEMY_SPEED);
        } else {
            vel.dx = 0;
            vel.dy = 0;
        }

    }

}
