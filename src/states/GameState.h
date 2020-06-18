#pragma once

#include "State.h"
#include <entity/registry.hpp>


class GameState : public State {
    std::vector<std::vector<entt::entity>> map;
    const int mapWidth;
    const int mapHeight;

public:
    GameState(int mapWidth, int mapHeight);


private:
    void initMap();
    void initPlayer();
};