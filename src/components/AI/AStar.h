#pragma once

#include <vector>
#include "../tilecomponents/TilePosition.h"
#include "../Position.h"
#include "../../util/Map.h"

namespace AStar {
    std::vector<TilePosition> pathfind(TilePosition start, TilePosition end, Map &map, entt::registry &registry);
}