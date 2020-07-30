#pragma once

#include <map>
#include "../../components/Sprites.h"
#include <SDL_scancode.h>

enum TileType {
    EMPTY = 0,
    WALL = 1,
    GOAL = 2,
    ENEMY_SPAWN = 3,
    WALLPRE = -1,
    TOWER_PROJECTILE = 4,
    TOWER_LASER = 5,
    TOWERPRE = -2,
};



int &getCostForType(TileType type);
std::shared_ptr<Sprite> &getSpriteForType(TileType type);
const std::map<SDL_Scancode, TileType> &getScancodeMap();
char* &getName(TileType type);


bool isSolid(TileType type);

bool isSpecial(TileType type);

