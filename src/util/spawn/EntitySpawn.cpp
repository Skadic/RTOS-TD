//
// Created by skadic on 09.07.20.
//

#include "EntitySpawn.h"
#include "../../components/Position.h"
#include "../GlobalConsts.h"
#include "../../components/Velocity.h"
#include "../../components/Sprites.h"
#include "../../components/Hitbox.h"
#include "../../components/Enemy.h"
#include "../../components/tags/Player.h"
#include "../../components/Health.h"
#include "../../components/tags/Projectile.h"

// Sprites
static std::shared_ptr<Sprite> ENEMY = std::make_shared<TextureSprite>("enemy_default_colors.png");
static std::shared_ptr<Sprite> PLAYER = std::make_shared<TextureSprite>("player.png");
static std::shared_ptr<Sprite> DEFAULT_PROJECTILE_SPRITE = std::make_shared<RectangleSprite>(DEFAULT_PROJECTILE_SIZE, DEFAULT_PROJECTILE_SIZE, 0xFF7000, true);

/**
 * The offset inside of a tile, needed so that a player or enemy spawns in the middle of a tile,
 * and not in the top left corner
 */
static const inline auto OFFSET_IN_TILE = TILE_SIZE / 2 - PLAYER_SIZE / 2;


entt::entity spawnEnemy(TilePosition spawnPos, entt::registry &registry, int health) {
    auto entity = registry.create();
    registry.emplace<Position>(entity, spawnPos.x * TILE_SIZE + OFFSET_IN_TILE, spawnPos.y * TILE_SIZE + OFFSET_IN_TILE);
    registry.emplace<Velocity>(entity, 0.0, 0.0, ENEMY_SPEED);
    registry.emplace<SpriteComponent>(entity, ENEMY);
    registry.emplace<Hitbox>(entity, PLAYER_SIZE, PLAYER_SIZE, true);
    registry.emplace<Enemy>(entity);
    registry.emplace<Health>(entity, health, health);
    return entity;
}

entt::entity spawnPlayer(TilePosition spawnPos, entt::registry &registry) {
    entt::entity player = registry.create();
    registry.emplace<Player>(player);
    registry.emplace<SpriteComponent>(player, PLAYER);
    registry.emplace<Position>(player, spawnPos.x * TILE_SIZE + OFFSET_IN_TILE, spawnPos.y * TILE_SIZE + OFFSET_IN_TILE);
    registry.emplace<Velocity>(player, 0.0, 0.0, PLAYER_SPEED);
    registry.emplace<Hitbox>(player, PLAYER_SIZE, PLAYER_SIZE, true);
    return player;
}

entt::entity spawnProjectile(Position spawnPos, entt::registry &registry) {
    entt::entity projectile = registry.create();
    registry.emplace<Projectile>(projectile);
    registry.emplace<SpriteComponent>(projectile, DEFAULT_PROJECTILE_SPRITE);
    registry.emplace<Position>(projectile, spawnPos.x, spawnPos.y);
    registry.emplace<Velocity>(projectile, 0.0, 0.0, DEFAULT_PROJECTILE_SPEED);
    registry.emplace<Hitbox>(projectile, DEFAULT_PROJECTILE_SIZE, DEFAULT_PROJECTILE_SIZE, false);
    return projectile;
}
