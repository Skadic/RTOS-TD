#pragma once

#include <vector>
#include <set>
#include <unordered_set>
#include <entity/entity.hpp>
#include <entity/registry.hpp>
#include <map>
#include "../components/tilecomponents/TilePosition.h"
#include "../components/Position.h"
#include "../components/Hitbox.h"
#include "../components/Enemy.h"

class CollisionTable {
    int width;
    int height;

    using entity_type = std::underlying_type_t<entt::entity>;

    /**
     * Each position in this 2D-vector contains all entities with a Position component which intersect the tile at said position
     */
    std::vector<std::vector<std::vector<entt::entity>>> tileBuckets;
    /**
     * Maps entities that contain ranges, to all the enemies in said range
     */
    std::map<entity_type, std::vector<entt::entity>> rangeBuckets;

    /**
     * Deletes the stored data
     */
    void resetAll();

    /**
     * Gets all tile positions that an entity with given position and hitbox would be intersecting
     * @return A vector of all said tiles
     */
    std::set<TilePosition> getIntersectingTiles(Position pos, Hitbox hitbox);

    /**
     * Gets all approximate tile positions that an entity with given position and radius would be intersecting
     * @return A vector of all said tiles
     */
    std::set<TilePosition> getIntersectingTilesApprox(Position pos, Range range);

    void resetRanges();
    void resetTiles();

public:

    CollisionTable(int w, int h);

    CollisionTable(CollisionTable const&) = delete;

    void operator=(CollisionTable const&) = delete;

    /**
     * Updates all data
     */
    void refreshAll(entt::registry &registry);

    /**
     * Gets all enemies, that are in the given entity's range
     * @param entity An entity, whose range to check.
     * @return All said enemies
     */
    std::vector<entt::entity> getEnemiesInRangeApprox(entt::entity &entity);

    /**
     * Gets all entities that are intersecting this tile
     * @return A vector of all entities that are intersecting this tile
     */
    std::vector<entt::entity>& getIntersectingEntities(TilePosition pos);

    /**
     * Gets all entities that are intersecting the tile at the specified position
     * @return A vector of all entities that are intersecting the tile at the specified position
     */
    std::vector<entt::entity>& getIntersectingEntities(int x, int y);

    void refreshRanges(entt::registry &registry);

    void refreshTiles(entt::registry &registry);
};
