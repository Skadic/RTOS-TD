#pragma once

#include "../../components/tilecomponents/TilePosition.h"
#include "../../components/AI/AI.h"
#include <glm.hpp>

entt::entity spawnEnemy(TilePosition spawnPos, entt::registry &registry, int health);

entt::entity spawnPlayer(TilePosition spawnPos, entt::registry &registry);

entt::entity spawnProjectile(Position spawnPos, entt::registry &registry);