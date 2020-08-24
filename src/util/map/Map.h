#pragma once

#include <vector>
#include <entity/registry.hpp>
#include "../sync/LockGuard.h"
#include "TileType.h"
#include "../../components/Sprites.h"
#include "../Renderer.h"
#include "../../components/tilecomponents/TilePosition.h"


class Map {
    /**
     * A 2D vector that contains the entities corresponding to the tiles at the specific x and y position in the map
     */
    std::vector<std::vector<entt::entity>> tiles;

    /**
     * The position of the nexus in the map
     */
    TilePosition nexus;

    /**
     * The position of the enemy spawn in the map
     */
    TilePosition spawn;

    /**
     * The width of the map
     */
    int mapWidth;

    /**
     * The height of the map
     */
    int mapHeight;

    /**
     * The path from the spawn to the nexus as a vector of tile positions
     */
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

    /**
     * Gets the entity which is at the specified position on the screen
     * @param x The screen x position to check
     * @param y The screen y position to check
     * @param renderer The renderer to transform the coordinates
     * @return The tile at the specified position wrapped in an optional if not outside the map. An empty optional otherwise.
     *
     * @see Renderer
     */
    std::optional<entt::entity> getMapTileAtScreenPos(short x, short y, Renderer &renderer);

    /**
     * Updates the Tile at the specified position in the map. Not all tile types are supported by this operation.
     * Not supported are special types like the nexus and the spawn.
     * @param x The map x coordinate to modify
     * @param y The map y coordinate to modify
     * @param registry The registry to retrieve and modify data
     * @param type The type to place at the position.
     */
    void updateTileAt(int x, int y, entt::registry &registry, TileType type);

    /**
     * Updates the Tile at the specified position on the screen. Not all tile types are supported by this operation.
     * Not supported are special types like the nexus and the spawn.
     * @param x The screen x position to modify
     * @param y The screen y position to modify
     * @param registry The registry to retrieve and modify data
     * @param type The type to place at the position.
     */
    void updateTileAtScreenPos(short x, short y, entt::registry &registry, TileType type, Renderer &renderer);


    /**
     * Gets the entity representing the tile at the specified map position
     * @param x The map x coordinate
     * @param y The map y coordinate
     * @return The entity representing the tile
     */
    entt::entity &getMapTile(int x, int y);

    /**
     * Gets the tile type at the specified map position
     * @param x The map x coordinate
     * @param y The map y coordinate
     * @param registry The registry to retrieve data from
     * @return The tile type at the specified position
     */
    TileType getTileTypeAt(int x, int y, entt::registry &registry);

    /**
     * Gets the tile type of the specified entity
     * @param tile The entity representing the tile to be checked
     * @param registry The registry to retrieve data from
     * @return The type of the given tile
     */
    static TileType getTileType(entt::entity tile, entt::registry &registry);

    /**
     * Recalculates the path through the map from the spawn to the nexus using the A* algorithm.
     * @param registry The registry to receive data from
     */
    void updateEnemyPath(entt::registry &registry);

private:
    /**
     * Initializes the boundaries of the map
     * The boundaries are 4 tiles that span each side of the map respectively.
     * These are used to keep entities from moving outside of the map
     * @param registry The registry to insert the entities into
     */
    void initBoundaries(entt::registry &registry);
};