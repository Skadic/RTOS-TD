//
// Created by skadic on 19.06.20.
//

#include <nlohmann/json.hpp>
#include <fstream>
#include "Map.h"
#include "../../components/tilecomponents/TilePosition.h"
#include "../../components/Sprites.h"
#include "TileType.h"
#include "../GlobalConsts.h"
#include "../../components/Hitbox.h"
#include "../../components/tilecomponents/TileTypeComponent.h"
#include "../../components/AI/AStar.h"
#include "../../components/tilecomponents/Tower.h"
#include "../../components/Damage.h"
#include "../../components/AI/tower/AreaOfEffectTowerAI.h"
#include "../../components/AI/tower/LaserTowerAI.h"
#include "../../components/Health.h"
#include "../../components/AI/tower/ProjectileTowerAI.h"
#include "../../components/tags/Nexus.h"
#include "../Log.h"
#include "../../components/Upgrade.h"
#include <map>

inline const auto MAP_DIR = RESOURCE_DIR + "maps/";

Map::Map(entt::registry &registry, int width, int height) : mapWidth{width}, mapHeight{height} {
    // Fill the map with empty tiles
    tiles.resize(mapHeight);
    for (int y = 0; y < mapHeight; ++y) {
        // Resize each row to the correct size
        tiles[y].resize(mapWidth);
        for (int x = 0; x < mapWidth; ++x) {
            // Create a new entity, give it the respective tile position, a tile type and a sprite
            entt::entity entity = registry.create();
            registry.emplace<TilePosition>(entity, x, y);
            registry.emplace<TileTypeComponent>(entity, EMPTY);
            registry.emplace<SpriteComponent>(entity, getSpriteForType(EMPTY));

            // Add the entity to the map grid
            tiles[y][x] = entity;
        }
    }

    // Add a enemy spawn in the top left corner
    spawn = TilePosition{0,0};
    registry.replace<TileTypeComponent>(tiles[spawn.y][spawn.x], ENEMY_SPAWN);
    registry.replace<SpriteComponent>(tiles[spawn.y][spawn.x], getSpriteForType(ENEMY_SPAWN));

    // Add an nexus in the bottom right corner
    nexus = TilePosition{static_cast<short>(width - 1),static_cast<short>(height - 1)};
    registry.replace<TileTypeComponent>(tiles[nexus.y][nexus.x], NEXUS);
    registry.replace<SpriteComponent>(tiles[nexus.y][nexus.x], getSpriteForType(NEXUS));
    registry.replace<Health>(tiles[nexus.y][nexus.x], NEXUS_HEALTH, NEXUS_HEALTH);

    // Add map boundaries, so entities cannot walk through the map borders
    initBoundaries(registry);
}

Map::Map(entt::registry &registry, std::string path) {
    // Prepend the map directory to the path.
    path.insert(0, MAP_DIR);
    std::ifstream file(path);

    if(!file.is_open()) {
        fatal("Map file could not be opened/found: " + path);
        return;
    }

    // Read the map json from the input stream
    nlohmann::json mapData;
    file >> mapData;
    file.close();

    // Get the width and height from the json data
    mapWidth = mapData["width"];
    mapHeight = mapData["height"];

    // A grid of ints stored in the map json
    // Each position contains a number that is equivalent to that number's representation as a TileType
    std::vector<std::vector<int>> tileData = mapData["tiledata"];

    // Resize to the correct amount of rows
    tiles.resize(mapHeight);

    for (int y = 0; y < mapHeight; ++y) {
        // Resize every row to the correct size
        tiles[y].resize(mapWidth);

        for (int x = 0; x < mapWidth; ++x) {
            // Create an entity
            entt::entity entity = registry.create();
            // Cast the int from the tile data to a TileType
            TileType type = static_cast<TileType>(tileData[y][x]);
            // Give the entity a tile position, a tile type and a sprite
            TilePosition &tilePos = registry.emplace<TilePosition>(entity, x, y);
            registry.emplace<TileTypeComponent>(entity, type);
            registry.emplace<SpriteComponent>(entity, getSpriteForType(type));

            // Any tile that is not empty has a hitbox
            if(type != EMPTY) registry.emplace<Hitbox>(entity, TILE_SIZE, TILE_SIZE);

            // If the tile at this position is the nexus, give it health and the nexus tag component
            if(type == NEXUS) {
                nexus = tilePos;
                registry.emplace<Health>(entity, NEXUS_HEALTH, NEXUS_HEALTH);
                registry.emplace<Nexus>(entity);
            }

            // Add the spawn position
            if(type == ENEMY_SPAWN) spawn = tilePos;


            // Add the entity to the map grid
            tiles[y][x] = entity;
        }
    }

    // Calculate the path from the spawn to the nexus
    Map::updateEnemyPath(registry);

    // Add map boundaries, so entities cannot walk through the map borders
    initBoundaries(registry);
}


int Map::getWidth() const {
    return mapWidth;
}

int Map::getHeight() const {
    return mapHeight;
}

entt::entity &Map::getMapTile(int x, int y) {
    return tiles[y][x];
}

std::optional<entt::entity> Map::getMapTileAtScreenPos(short xPos, short yPos, Renderer &renderer) {
    // Transform the screen position to in game position, and calculate the tile position at that in game position
    short x = renderer.getGameX(xPos) / TILE_SIZE;
    short y = renderer.getGameY(yPos) / TILE_SIZE;

    // If the calculated position is within the map bounds, return the entity representing the tile at that position
    // otherwise return an empty optional
    if(x >= 0 && y >= 0 && x < mapWidth && y < mapHeight) {
        return std::make_optional<entt::entity>(getMapTile(x, y));
    } else {
        return std::optional<entt::entity>{};
    }
}

TileType Map::getTileTypeAt(int x, int y, entt::registry &registry) {
    // If the position is outside the map bounds, return EMPTY
    if(x < 0 || x >= mapWidth || y < 0 || y >= mapHeight) return EMPTY;
    // otherwise return the tile type of the tile at the given position
    return registry.get<TileTypeComponent>(getMapTile(x, y)).type;
}

void updateTile(entt::entity tile, entt::registry &registry, TileType type) {
    // Give the tile the appropriate type and sprite
    registry.emplace_or_replace<TileTypeComponent>(tile, type);
    registry.emplace_or_replace<SpriteComponent>(tile, getSpriteForType(type));
    // If the tile is solid, it will get a solid hitbox, otherwise a non-solid one
    registry.emplace_or_replace<Hitbox>(tile, TILE_SIZE, TILE_SIZE, isSolid(type));

    // For each type, give the entity the appropriate components
    switch (type) {
        case TOWER_PROJECTILE: {
            registry.emplace_or_replace<Range>(tile, PROJECTILE_TOWER_RANGE);
            registry.emplace_or_replace<Tower>(tile);
            registry.emplace_or_replace<Damage>(tile, PROJECTILE_TOWER_DAMAGE);
            registry.emplace_or_replace<Upgrade>(tile, PROJECTILE_TOWER_DAMAGE_SCALING);
            registry.emplace_or_replace<AIComponent>(tile, new ProjectileTowerAI(tile, DEFAULT_PROJECTILE_SPEED, PROJECTILE_FIRE_INTERVAL));
            break;
        }
        case TOWER_LASER: {
            registry.emplace_or_replace<Range>(tile, LASER_TOWER_RANGE);
            registry.emplace_or_replace<Tower>(tile);
            registry.emplace_or_replace<Damage>(tile, LASER_TOWER_DAMAGE);
            registry.emplace_or_replace<Upgrade>(tile, LASER_TOWER_DAMAGE_SCALING);
            registry.emplace_or_replace<AIComponent>(tile, new LaserTowerAI(tile));
            break;
        }
        case TOWER_AOE: {
            registry.emplace_or_replace<Range>(tile, AOE_TOWER_RANGE);
            registry.emplace_or_replace<Tower>(tile);
            registry.emplace_or_replace<Damage>(tile, AOE_TOWER_DAMAGE);
            registry.emplace_or_replace<Upgrade>(tile, AOE_TOWER_DAMAGE_SCALING);
            registry.emplace_or_replace<AIComponent>(tile, new AreaOfEffectTowerAI(tile, AOE_FIRE_INTERVAL));
            break;
        }
        // If the type to be placed is not a tower, remove the unneeded components
        default: {
            registry.remove_if_exists<Range, Tower, Damage, AIComponent, Upgrade>(tile);
        }
    }
}

void Map::updateTileAt(int x, int y, entt::registry &registry, TileType type) {
    // Gets the entity at the position and updates it. See updateTile
    entt::entity entity = getMapTile(x, y);
    updateTile(entity, registry, type);
}

void Map::updateTileAtScreenPos(short x, short y, entt::registry &registry, TileType type, Renderer &renderer) {
    std::optional<entt::entity> entity = getMapTileAtScreenPos(x, y, renderer);
    if(!entity) fatal("No tile at screen position: " + std::to_string(x) + ", " + std::to_string(y));
    updateTile(*entity, registry, type);
}

TilePosition Map::getNexusPosition() const {
    return nexus;
}

TileType Map::getTileType(entt::entity tile, entt::registry &registry) {
    return registry.get<TileTypeComponent>(tile).type;
}

TilePosition Map::getSpawnPosition() const {
    return spawn;
}

std::vector<TilePosition> &Map::getPath() {
    return enemyPath;
}

void Map::updateEnemyPath(entt::registry &registry) {
    enemyPath = AStar::pathfind(spawn, nexus, *this, registry);
}

void Map::initBoundaries(entt::registry &registry) {
    // Create a boundary for each side of the map, which exist to block entities from moving out of the map
    entt::entity left = registry.create();
    registry.emplace<TilePosition>(left, -1, 0);
    registry.emplace<Hitbox>(left, TILE_SIZE, mapHeight * TILE_SIZE, true);

    entt::entity top = registry.create();
    registry.emplace<TilePosition>(top, 0, -1);
    registry.emplace<Hitbox>(top, TILE_SIZE * mapWidth, TILE_SIZE, true);

    entt::entity right = registry.create();
    registry.emplace<TilePosition>(right, mapWidth, 0);
    registry.emplace<Hitbox>(right, TILE_SIZE, TILE_SIZE * mapHeight, true);

    entt::entity bottom = registry.create();
    registry.emplace<TilePosition>(bottom, 0, mapHeight);
    registry.emplace<Hitbox>(bottom, TILE_SIZE * mapWidth, TILE_SIZE, true);
}
