#pragma once

#include <vector>
#include <entity/registry.hpp>
#include "../sync/LockGuard.h"
#include "TileType.h"
#include "../../components/Sprites.h"
#include "../Renderer.h"
#include "../../components/tilecomponents/TilePosition.h"


class Map {
    std::vector<std::vector<entt::entity>> tiles;
    TilePosition nexus;
    TilePosition spawn;
    int mapWidth;
    int mapHeight;
    std::vector<TilePosition> enemyPath;


public:
    Map(entt::registry &regMutex, int width, int height);
    Map(entt::registry &regMutex, std::string path);
    int getWidth() const;
    int getHeight() const;
    TilePosition getNexusPosition() const;
    TilePosition getSpawn() const;
    std::vector<TilePosition> &getPath();

    std::optional<entt::entity> getMapTileAtScreenPos(short x, short y, Renderer &renderer);

    void updateTileAt(int x, int y, entt::registry &registry, TileType type);
    void updateTileAtScreenPos(short x, short y, entt::registry &registry, TileType type, Renderer &renderer);

    entt::entity &getMapTile(int x, int y);
    TileType getTileTypeAt(int x, int y, entt::registry &registry);
    TileType getTileType(entt::entity tile, entt::registry &registry);
    void updateEnemyPath(entt::registry &registry);

private:
    void initBoundaries(entt::registry &registry);
};