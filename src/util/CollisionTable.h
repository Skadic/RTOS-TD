#pragma once

#include <vector>
#include <set>
#include <unordered_set>
#include <entity/entity.hpp>
#include <entity/registry.hpp>
#include "../components/tilecomponents/TilePosition.h"
#include "../components/Position.h"
#include "../components/Hitbox.h"

class CollisionTable {
    int width;
    int height;

    /**
     * Each position in this 2D-vector contains all entities with a Position component which intersect the tile at said position
     */
    std::vector<std::vector<std::vector<entt::entity>>> buckets;

    /**
     * Deletes the stored data
     */
    void reset();

public:

    CollisionTable(int w, int h);
    CollisionTable(CollisionTable const&) = delete;
    void operator=(CollisionTable const&) = delete;

    /**
     * Updates all data
     */
    void refresh(entt::registry &registry);

    /**
     * Gets all entities that are intersecting this tile
     * @return A vector of all entities that are intersecting this tile
     */
    std::vector<entt::entity>& getIntersecting(TilePosition pos);

    /**
     * Gets all entities that are intersecting the tile at the specified position
     * @return A vector of all entities that are intersecting the tile at the specified position
     */
    std::vector<entt::entity>& getIntersecting(int x, int y);

    /**
     * Gets all tile positions that an entity with given position and hitbox would be intersecting
     * @return A vector of all said tiles
     */
    std::vector<TilePosition> getIntersectingTiles(Position pos, Hitbox hitbox);
};
