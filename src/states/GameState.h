#pragma once

#include "State.h"
#include "../util/Map.h"
#include <entity/registry.hpp>


class GameState : public State {
    Map map;

public:
    // Initializes the game state with an empty map of the given size
    GameState(int mapWidth, int mapHeight);
    // Initializes the game state with a map read from a file
    explicit GameState(std::string mapPath);


    Map &getMap();

private:
    void initPlayer();
    void initEnemy();
    void initTasks();
};