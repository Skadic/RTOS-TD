#pragma once

#include <vector>
#include "../tilecomponents/TilePosition.h"
#include "../Position.h"
#include "../../util/map/Map.h"

/**
 * This contains the A* path-finding algorithm
 */
namespace AStar {
    /**
     * Find the shortest path between 2 Tiles using the A* algorithm
     * @param start The start point
     * @param end The end point
     * @param map The map which to traverse through
     * @param registry The registry which to retrieve data from
     * @return A shortest path from start to end as a vector of tile positions, or an empty vector, if there is no such path.
     */
    std::vector<TilePosition> pathfind(TilePosition start, TilePosition end, Map &map, entt::registry &registry);
}