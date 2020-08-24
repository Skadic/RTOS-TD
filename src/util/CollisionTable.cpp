//
// Created by skadic on 29.07.20.
//

#include "CollisionTable.h"
#include "GlobalConsts.h"
#include "map/TileType.h"

const auto TILE_DIAGONAL = std::sqrt(2) * TILE_SIZE;


CollisionTable::CollisionTable(int w, int h) : width{w}, height{h} {
    boundaryBuckets.resize(Direction::LAST_DIRECTION);
    tileBuckets.resize(height);
    for (int i = 0; i < height; ++i) {
        tileBuckets[i].resize(width);
    }
}

void CollisionTable::resetAll() {
    resetTiles();
    resetRanges();
}

void CollisionTable::resetTiles() {
    for (int i = 0; i < height; ++i) {
        tileBuckets[i].clear();
        tileBuckets[i].resize(width);
    }
    boundaryBuckets.clear();
}

void CollisionTable::resetRanges() {
    rangeBuckets.clear();
}

void CollisionTable::refreshAll(entt::registry &registry) {
    refreshTiles(registry);
    refreshRanges(registry);
}

void CollisionTable::refreshTiles(entt::registry &registry) {
    resetTiles();
    auto view = registry.view<Position, Hitbox>();

    for(auto &entity : view) {
        Position &pos = view.get<Position>(entity);
        Hitbox &hitbox = view.get<Hitbox>(entity);

        // Get all tiles that are intersecting with the hitbox of the current entity
        auto intersecting = getIntersectingTiles(pos, hitbox);


        for(TilePosition tPos : intersecting) {
            // The boundaries have special tile positions outside of the map.
            // If that is the case, check them and insert the current entity into the respective bucket
            if(tPos.x < 0) boundaryBuckets[LEFT].push_back(entity);
            else if(tPos.x >= width) boundaryBuckets[RIGHT].push_back(entity);
            if(tPos.y < 0) boundaryBuckets[UP].push_back(entity);
            else if(tPos.y >= height) boundaryBuckets[DOWN].push_back(entity);
            else { // If the tile position is on the map, insert the entity into the respective bucket
                tileBuckets[tPos.y][tPos.x].push_back(entity);
            }
        }
    }
}

void CollisionTable::refreshRanges(entt::registry &registry) {
    resetRanges();

    auto enemyView = registry.view<Enemy>();

    // Handling non-tiles with ranges
    auto posRangeView = registry.view<Position, Range>();

    for(auto &entity : posRangeView) {
        Position &pos = posRangeView.get<Position>(entity);
        Range &range = posRangeView.get<Range>(entity);
        // If there is no vector yet to hold the enemies that intersect this range, insert one
        if(rangeBuckets.find(entt::to_integral(entity)) != rangeBuckets.end()) {
            rangeBuckets[entt::to_integral(entity)] = std::vector<entt::entity>();
        }
        auto &enemyVec = rangeBuckets[entt::to_integral(entity)];

        // Get all the approximate tiles, that the range of this entity is intersecting
        auto intersecting = getIntersectingTilesApprox(pos, range);
        for(TilePosition tPos : intersecting) {
            // Check that the tile Position is in range
            if (tPos.x >= 0 && tPos.x < width && tPos.y >= 0 && tPos.y < height && tPos.toPosition().distance(pos) <= range.radius + TILE_DIAGONAL) {

                // Check if each entity is actually an enemy, and if so, add to the vector
                for (auto enemy : getIntersectingEntities(tPos)) {
                    if(enemyView.contains(enemy)) {
                        enemyVec.push_back(enemy);
                    }
                }
            }
        }
    }


    // Handling Tiles with Ranges

    auto tileposRangeView = registry.view<TilePosition, Range>();

    for(auto &entity : tileposRangeView) {
        Position pos = tileposRangeView.get<TilePosition>(entity).toPosition();
        Range &range = tileposRangeView.get<Range>(entity);

        // If there is no vector yet to hold the enemies that intersect this range, create one
        if(rangeBuckets.find(entt::to_integral(entity)) != rangeBuckets.end()) {
            rangeBuckets[entt::to_integral(entity)] = std::vector<entt::entity>();
        }
        auto &enemyVec = rangeBuckets[entt::to_integral(entity)];

        // Get all the approximate tiles, that the range of this entity is intersecting
        auto intersecting = getIntersectingTilesApprox(pos, range);
        for(TilePosition tPos : intersecting) {
            // Check that the tile Position is in range
            if (tPos.x >= 0 && tPos.x < width && tPos.y >= 0 && tPos.y < height && tPos.toPosition().distance(pos) <= range.radius + TILE_DIAGONAL) {

                for (auto enemy : getIntersectingEntities(tPos)) {
                    if(enemyView.contains(enemy)) {
                        enemyVec.push_back(enemy);
                    }
                }
            }
        }
    }
}

std::set<TilePosition> CollisionTable::getIntersectingTiles(Position pos, Hitbox hitbox) {
    std::set<TilePosition> posSet;

    // Calculate the tiles on which the top left corner and the bottom right corner sit respectively
    TilePosition topLeft = {static_cast<short>(pos.x / TILE_SIZE), static_cast<short>(pos.y / TILE_SIZE)};
    TilePosition bottomRight = {static_cast<short>((pos.x + hitbox.width) / TILE_SIZE), static_cast<short>((pos.y + hitbox.height) / TILE_SIZE)};

    // Each tile in the rectangle defined by the two tile positions is intersected by the hitbox
    for (short y = topLeft.y; y <= bottomRight.y; ++y) {
        for (short x = topLeft.x; x <= bottomRight.x; ++x) {
            if (x >= 0 && x < width && y >= 0 && y < height) {
                posSet.insert({x, y});
            }
        }
    }

    // Handle the boundaries
    if(pos.x < 0) posSet.insert({-1, 0}); // Left boundary
    else if(pos.x > width * TILE_SIZE) posSet.insert({static_cast<short>(width), 0}); // Right boundary
    if(pos.y < 0) posSet.insert({0, -1}); // Top boundary
    else if(pos.y > height * TILE_SIZE) posSet.insert({0, static_cast<short>(height)}); // Bottom boundary

    return posSet;
}

std::set<TilePosition> CollisionTable::getIntersectingTilesApprox(Position pos, Range range) {
    // Rough approximation. The approximate intersecting tiles are calculated, by calculating the tiles
    // which are intersected by the square surrounding the (circular) range
    Position newPos = {pos.x - range.radius, pos.y - range.radius};
    Hitbox box = {range.radius * 2, range.radius * 2};
    return getIntersectingTiles(newPos, box);
}


std::vector<entt::entity>& CollisionTable::getIntersectingEntities(int x, int y) {
    // Handle the boundaries, which use out-of-bounds coordinates to be addressed
    if(x < 0) return boundaryBuckets[LEFT];
    else if(x >= width) return boundaryBuckets[RIGHT];
    if(y < 0) return boundaryBuckets[UP];
    else if(y >= height) return boundaryBuckets[DOWN];

    // Otherwise just return the contents of the respective buckets
    return tileBuckets[y][x];
}

std::vector<entt::entity>& CollisionTable::getIntersectingEntities(TilePosition pos) {
    return getIntersectingEntities(pos.x, pos.y);
}

std::vector<entt::entity> CollisionTable::getEnemiesInRangeApprox(entt::entity &entity) {
    return rangeBuckets[entt::to_integral(entity)];
}


