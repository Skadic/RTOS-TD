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
    /**
     * Creates a map with empty tiles of the specified width
     * @param registry The registry to which to add the entities
     * @param width The width of the map
     * @param height The height of the map
     */
    Map(entt::registry &registry, int width, int height);

    /**
     * Creates a map from a json file
     * @param registry The registry to which to add the entities
     * @param path The path to the json file
     */
    Map(entt::registry &registry, std::string path);


    int getWidth() const;
    int getHeight() const;
    TilePosition getNexusPosition() const;
    TilePosition getSpawnPosition() const;

    /**
     * Gets the path from the enemy spawn to the nexus as a vector of tile positions
     * If there is no such path, this should be empty
     */
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