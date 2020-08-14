#pragma once

#include "../../components/tilecomponents/TilePosition.h"
#include "../../components/AI/AI.h"
#include <glm.hpp>

/**
 * Spawns an enemy at the specified tile position without an AI
 * @param spawnPos The position at which the enemy should spawn
 * @param registry The registry in which to insert the entity representing the enemy
 * @param health The enemy's health
 * @return The entity representing the enemy
 */
entt::entity spawnEnemy(TilePosition spawnPos, entt::registry &registry, int health);

/**
 * Spawns the player at the specified tile position
 * @param spawnPos The position at which the player should spawn
 * @param registry The registry in which to insert the entity representing the player
 * @return The entity representing the player
 */
entt::entity spawnPlayer(TilePosition spawnPos, entt::registry &registry);

/**
 * Spawns a projectile at the specified position without an AI
 * @param spawnPos The position at which the projectile should spawn
 * @param registry The registry in which to insert the entity representing the projectile
 * @return The entity representing the projectile
 */
entt::entity spawnProjectile(Position spawnPos, entt::registry &registry);