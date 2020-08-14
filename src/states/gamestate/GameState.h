#pragma once

#include "../State.h"
#include "../../util/map/Map.h"
#include "../../util/Wave.h"
#include "../../util/CollisionTable.h"
#include <entity/registry.hpp>


class GameState : public State {
    Map map;
    Wave wave;
    CollisionTable collisionTable;
    TileType tileTypeToPlace;
    int coins;

public:

    /**
     * Initializes the game state with an empty map of the given size
     * @param mapWidth The width of the map
     * @param mapHeight The height of the map
     */
    GameState(int mapWidth, int mapHeight);
    // Initializes the game state with a map read from a file
    explicit GameState(std::string mapPath);

    CollisionTable &getCollisionTable();

    Map &getMap();
    Wave &getWave();
    void setWave(const Wave &wave);

    int getCoins() const;
    void setCoins(int coins);
    void addCoins(int coins);
    void removeCoins(int coins);

    void setTileTypeToPlace(TileType tileTypeToPlace);
    TileType getTileTypeToPlace() const;

private:
    void initTasks();
};