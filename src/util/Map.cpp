//
// Created by skadic on 19.06.20.
//

#include <nlohmann/json.hpp>
#include <fstream>
#include "Map.h"
#include "../components/TilePosition.h"
#include "../components/Sprites.h"
#include "TileType.h"
#include "GlobalConsts.h"
#include <map>

std::map<TileType, std::shared_ptr<Sprite>> initTileSprites() {
    std::map<TileType, std::shared_ptr<Sprite>> map;
    map[EMPTY] = std::make_shared<EmptySprite>();
    map[WALL] = std::make_shared<RectangleSprite>(TILE_SIZE, TILE_SIZE, 0xFFFFFF, false);
    map[TOWER] = std::make_shared<RectangleSprite>(TILE_SIZE, TILE_SIZE, 0x00FFFF, true);
    return map;
}

std::map<TileType, std::shared_ptr<Sprite>> TILE_SPRITE_MAP = initTileSprites();

Map::Map(entt::registry &registry, int width, int height) : mapWidth{width}, mapHeight{height} {
    tiles.resize(mapHeight);
    for (int y = 0; y < mapHeight; ++y) {
        tiles[y].resize(mapWidth);
        for (int x = 0; x < mapWidth; ++x) {
            entt::entity entity = registry.create();
            registry.emplace<TilePosition>(entity, x, y);
            registry.emplace<SpriteComponent>(entity, TILE_SPRITE_MAP[EMPTY]);

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

    tiles.resize(mapHeight);
    for (int y = 0; y < mapHeight; ++y) {
        tiles[y].resize(mapWidth);
        for (int x = 0; x < mapWidth; ++x) {
            entt::entity entity = registry.create();
            TileType type = static_cast<TileType>(tileData[y][x]);
            registry.emplace<TilePosition>(entity, x, y);
            registry.emplace<SpriteComponent>(entity, TILE_SPRITE_MAP[type]);

            tiles[y][x] = entity;
        }
    }
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

std::optional<entt::entity> Map::getMapTileFromScreenPos(short xPos, short yPos, Renderer &renderer) {
    short x = renderer.reverseTransformX(xPos) / TILE_SIZE;
    short y = renderer.reverseTransformY(yPos) / TILE_SIZE;

    if(x >= 0 && y >= 0 && x < mapWidth && y < mapHeight) {
        return std::make_optional<entt::entity>(getMapTile(x, y));
    } else {
        return std::optional<entt::entity>{};
    }
}

std::shared_ptr<Sprite> getSpriteForType(TileType type) {
    return TILE_SPRITE_MAP[type];
}
