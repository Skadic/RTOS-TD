#pragma once

#include <vector>
#include "../TilePosition.h"
#include "../Position.h"
#include "../../util/Map.h"

std::vector<TilePosition> aStarPathfinding(TilePosition start, TilePosition end, Map &map, entt::registry &registry);