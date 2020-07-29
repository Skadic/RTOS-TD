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

    void reset();

public:

    CollisionTable(int w, int h);

    void refresh(entt::registry &registry);
    std::vector<entt::entity>& getIntersecting(TilePosition pos);
    std::vector<entt::entity>& getIntersecting(int x, int y);

    std::vector<TilePosition> getIntersectingTiles(Position pos, Hitbox hitbox);
};
