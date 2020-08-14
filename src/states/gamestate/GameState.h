#pragma once

#include "../State.h"
#include "../../util/map/Map.h"
#include "../../util/Wave.h"
#include "../../util/CollisionTable.h"
#include <entity/registry.hpp>


class GameState : public State {
    /**
     * The map which the game takes place on
     */
    Map map;

    /**
     * The current wave of enemies
     */
    Wave wave;

    /**
     * The CollisionTable for this map
     */
    CollisionTable collisionTable;

    /**
     * The current tile type that is to be placed with a left click
     */
    TileType tileTypeToPlace;

    /**
     * The current amount of coins, the player owns
     */
    int coins;

public:

    /**
     * Initializes the game state with an empty map of the given size
     * @param mapWidth The width of the map
     * @param mapHeight The height of the map
     */
    GameState(int mapWidth, int mapHeight);


    /**
     * Initializes the game state with a map read from a file
     * @param mapPath The path to the map json file
     */
    explicit GameState(std::string mapPath);

    CollisionTable &getCollisionTable();

    Map &getMap();
    Wave &getWave();
    void setWave(const Wave &wave);

    int getCoins() const;
    void setCoins(int coins);
    void addCoins(int coins);
    void removeCoins(int coins);

    /**
     * Sets the tile type that is to be placed by a left click
     */
    void setTileTypeToPlace(TileType tileTypeToPlace);
    TileType getTileTypeToPlace() const;

private:
    /**
     * Initializes the tasks the game state needs to run
     */
    void initTasks();
};