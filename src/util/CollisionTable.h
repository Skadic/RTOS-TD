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

/**
 * This class handles broad phase collision detection.
 * For a map with width `w` and height `h`, a CollisionTable of equal size should be created.
 * Each entity is inserted into entries `tileBuckets[y][x]` if and only if the entity's hitbox intersects with the
 * tile at the map coordinates (x,y). This creates a rough estimate of which entities might be colliding with each other.
 *
 * For the circular ranges, first all tiles that potentially intersect the range are calculated. This is done by calculating
 * all tiles that intersect the square that surrounds the range. For each of these tiles, all previously calculated intersecting entities
 * are retrieved from the `tileBuckets` vector, and added to the range's entry in `rangeBuckets`.
 * This way, we have a mapping from entities with ranges, to all potentially intersecting entities.
 */
class CollisionTable {
    /**
     * The width of the map, this collision table is for
     */
    int width;

    /**
     * The height of the map, this collision table is for
     */
    int height;

    /**
     * The underlying type of entt::entities, used for maps
     */
    using entity_type = std::underlying_type_t<entt::entity>;


    /**
     * Each of these buckets contains all entities which intersect which each of the respective map boundaries
     * Each of those boundaries are single tiles which span their respective side of the map
     * This vector should be indexed with the Direction enum to get the data related to the boundary of the particular side of the map
     * e.g. `boundaryBuckets[RIGHT]` returns the vector of all entities, which intersect with the right boundary of the map
     */
    std::vector<std::vector<entt::entity>> boundaryBuckets;

    /**
     * Each position in this 2D-vector contains all entities with a Position component which intersect the tile at said position
     */
    std::vector<std::vector<std::vector<entt::entity>>> tileBuckets;
    /**
     * Maps entities that contain ranges, to all the enemies in said range
     */
    std::map<entity_type, std::vector<entt::entity>> rangeBuckets;

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

    /**
     * Clears all data related to ranges
     */
    void resetRanges();

    /**
     * Clears all data related to tiles
     */
    void resetTiles();


public:

    /**
     * Clears all stored data
     */
    void resetAll();

    /**
     * Creates an empty Collision table with the specified size
     * @param w The width of the table
     * @param h The height of the table
     */
    CollisionTable(int w, int h);

    /**
     * The CollisionTable cannot be copied
     */
    CollisionTable(CollisionTable const&) = delete;

    /**
     * The CollisionTable cannot be copied
     */
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

    /**
     * Refreshes all data related to ranges
     * @param registry The registry to pull data from
     */
    void refreshRanges(entt::registry &registry);

    /**
     * Refreshes all data related to tiles
     * @param registry The registry to pull data from
     */
    void refreshTiles(entt::registry &registry);
};
