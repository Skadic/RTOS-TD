#pragma once

#include "State.h"
#include <entity/registry.hpp>


class GameState : public State {
    std::vector<std::vector<entt::entity>> map;
    const int mapWidth;
    const int mapHeight;

public:
    GameState(int mapWidth, int mapHeight);

    const int getMapWidth() const;
    const int getMapHeight() const;

    std::optional<entt::entity> getMapTileFromScreenPos(short x, short y);

    entt::entity &getMapTile(int x, int y);

private:
    void initMap();
    void initPlayer();
};