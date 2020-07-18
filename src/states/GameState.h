#pragma once

#include "State.h"
#include "../util/map/Map.h"
#include "../util/Wave.h"
#include <entity/registry.hpp>


class GameState : public State {
    Map map;
    Wave wave;
    int coins;
public:
    TileType getTileTypeToPlace() const;

private:
    TileType tileTypeToPlace;
public:
    int getCoins() const;

public:
    // Initializes the game state with an empty map of the given size
    GameState(int mapWidth, int mapHeight);
    // Initializes the game state with a map read from a file
    explicit GameState(std::string mapPath);


    Map &getMap();
    Wave &getWave();

    void setWave(const Wave &wave);

    void setCoins(int coins);

    void setTileTypeToPlace(TileType tileTypeToPlace);

private:
    void initTasks();
};