//
// Created by skadic on 19.06.20.
//

#include <nlohmann/json.hpp>
#include <fstream>
#include "Map.h"
#include "../components/tilecomponents/TilePosition.h"
#include "../components/Sprites.h"
#include "TileType.h"
#include "GlobalConsts.h"
#include "../components/Hitbox.h"
#include "../components/tilecomponents/TileTypeComponent.h"
#include "../components/AI/AStar.h"
#include "../components/tilecomponents/Tower.h"
#include "../components/Damage.h"
#include "../components/AI/tower/AreaOfEffectTowerAI.h"
#include <map>

Map::Map(entt::registry &registry, int width, int height) : mapWidth{width}, mapHeight{height} {
    tiles.resize(mapHeight);
    for (int y = 0; y < mapHeight; ++y) {
        tiles[y].resize(mapWidth);
        for (int x = 0; x < mapWidth; ++x) {
            entt::entity entity = registry.create();
            registry.emplace<TilePosition>(entity, x, y);
            registry.emplace<TileTypeComponent>(entity, EMPTY);
            registry.emplace<SpriteComponent>(entity, getSpriteForType(EMPTY));

            tiles[y][x] = entity;
        }
    }
}

Map::Map(entt::registry &registry, std::string path) {
    path.insert(0, "../resources/maps/");
    std::ifstream file;

    file.open(path);

    if(!file) {
        std::cerr << "BRUH" << std::endl;
        return;
    }

    nlohmann::json mapData;
    file >> mapData;
    file.close();

    mapWidth = mapData["width"];
    mapHeight = mapData["height"];

    std::vector<std::vector<int>> tileData = mapData["tiledata"];

    // Resize to the correct amount of rows
    tiles.resize(mapHeight);

    for (int y = 0; y < mapHeight; ++y) {
        // Resize every row to the correct size
        tiles[y].resize(mapWidth);

        for (int x = 0; x < mapWidth; ++x) {
            entt::entity entity = registry.create();
            TileType type = static_cast<TileType>(tileData[y][x]);
            TilePosition &tilePos = registry.emplace<TilePosition>(entity, x, y);
            registry.emplace<TileTypeComponent>(entity, type);
            registry.emplace<SpriteComponent>(entity, getSpriteForType(type));

            if(type != EMPTY) registry.emplace<Hitbox>(entity, TILE_SIZE, TILE_SIZE);
            if(type == GOAL) nexus = tilePos;
            if(type == ENEMY_SPAWN) spawn = tilePos;
            tiles[y][x] = entity;
        }
    }

    enemyPath = AStar::pathfind(spawn, nexus, *this, registry);
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
    short x = renderer.reverseTransformX(xPos) / TILE_SIZE;
    short y = renderer.reverseTransformY(yPos) / TILE_SIZE;

    if(x >= 0 && y >= 0 && x < mapWidth && y < mapHeight) {
        return std::make_optional<entt::entity>(getMapTile(x, y));
    } else {
        return std::optional<entt::entity>{};
    }
}

TileType Map::getTileTypeAt(int x, int y, entt::registry &registry) {
    if(x < 0 || x >= mapWidth || y < 0 || y >= mapHeight) return EMPTY;
    return registry.get<TileTypeComponent>(getMapTile(x, y)).type;
}

void updateTile(entt::entity tile, entt::registry &registry, TileType type) {
    registry.emplace_or_replace<TileTypeComponent>(tile, type);
    registry.emplace_or_replace<SpriteComponent>(tile, getSpriteForType(type));
    if(isSolid(type)) {
        registry.emplace_or_replace<Hitbox>(tile, TILE_SIZE, TILE_SIZE);
    } else if(type == EMPTY) {
        registry.remove_if_exists<Hitbox>(tile);
    }

    if(type == TOWER) {
        registry.emplace_or_replace<Range>(tile, 3 * TILE_SIZE);
        registry.emplace_or_replace<Tower>(tile);
        registry.emplace_or_replace<Damage>(tile, 1);
        registry.emplace_or_replace<AIComponent>(tile, new AreaOfEffectTowerAI(tile, 10, registry));
    } else {
        registry.remove_if_exists<Range>(tile);
        registry.remove_if_exists<Tower>(tile);
        registry.remove_if_exists<Damage>(tile);
        registry.remove_if_exists<AIComponent>(tile);
    }
}

void Map::updateTileAt(int x, int y, entt::registry &registry, TileType type) {
    entt::entity entity = getMapTile(x, y);
    updateTile(entity, registry, type);
}

void Map::updateTileAtScreenPos(short x, short y, entt::registry &registry, TileType type, Renderer &renderer) {
    entt::entity entity = *getMapTileAtScreenPos(x, y, renderer);
    updateTile(entity, registry, type);
}

TilePosition Map::getNexus() const {
    return nexus;
}

TileType Map::getTileType(entt::entity tile, entt::registry &registry) {
    return registry.get<TileTypeComponent>(tile).type;
}

TilePosition Map::getSpawn() const {
    return spawn;
}

std::vector<TilePosition> &Map::getPath() {
    return enemyPath;
}
