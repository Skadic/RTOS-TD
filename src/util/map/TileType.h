#pragma once


#include "../../components/Sprites.h"

enum TileType {
    EMPTY = 0,
    WALL = 1,
    TOWER = 2,
    GOAL = 3,
    ENEMY_SPAWN = 4,
};

std::shared_ptr<Sprite> &getSpriteForType(TileType type);

bool isSolid(TileType type);