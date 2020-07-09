//
// Created by skadic on 09.07.20.
//

#include "EntitySpawn.h"
#include "../../components/Position.h"
#include "../GlobalConsts.h"
#include "../../components/Velocity.h"
#include "../../components/Sprites.h"
#include "../../components/Hitbox.h"
#include "../../components/tags/Enemy.h"
#include "../../components/tags/Player.h"
#include "../../components/Health.h"

static std::shared_ptr<Sprite> ENEMY = std::make_shared<RectangleSprite>(PLAYER_SIZE, PLAYER_SIZE, 0xFF0000, true);
static std::shared_ptr<Sprite> PLAYER = std::make_shared<RectangleSprite>(PLAYER_SIZE, PLAYER_SIZE, 0x00FF00, true);

entt::entity spawnEnemy(TilePosition spawnPos, entt::registry &registry, int health) {
    auto entity = registry.create();
    registry.emplace<Position>(entity, spawnPos.x * TILE_SIZE + TILE_SIZE / 2 - PLAYER_SIZE / 2, spawnPos.y * TILE_SIZE  + TILE_SIZE / 2 - PLAYER_SIZE / 2);
    registry.emplace<Velocity>(entity, 0, 0);
    registry.emplace<SpriteComponent>(entity, ENEMY);
    registry.emplace<Hitbox>(entity, PLAYER_SIZE, PLAYER_SIZE);
    registry.emplace<Enemy>(entity);
    registry.emplace<Health>(entity, health, health * 3/4);
    return entity;
}

entt::entity spawnPlayer(TilePosition spawnPos, entt::registry &registry) {
    entt::entity player = registry.create();
    registry.emplace<Player>(player);
    registry.emplace<SpriteComponent>(player, PLAYER);
    registry.emplace<Position>(player, spawnPos.x * TILE_SIZE + TILE_SIZE / 2 - PLAYER_SIZE / 2, spawnPos.y * TILE_SIZE  + TILE_SIZE / 2 - PLAYER_SIZE / 2);
    registry.emplace<Velocity>(player, 0.0, 0.0);
    registry.emplace<Hitbox>(player, PLAYER_SIZE, PLAYER_SIZE);
    return player;
}
