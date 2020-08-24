#pragma once

#include <map>
#include "../../components/Sprites.h"
#include <SDL_scancode.h>

/**
 * All the types of tiles that a tile can hold
 */
enum TileType {
    EMPTY,
    WALL,
    NEXUS,
    ENEMY_SPAWN,
    TOWER_PROJECTILE,
    TOWER_LASER,
    TOWER_AOE,
    LAST_TILE_TYPE // Constant, only used to initialize Arrays etc. Not to be used. Add new enum constants only BEFORE this one
};



/**
 * @param type A type to check
 * @return The amount of coins needed to build this Tower
 */
int getCostForType(TileType type);

/**
 * Gets the sprite associated with this tile type
 * @param type A tile type to check
 * @return The type's sprite
 */
std::shared_ptr<Sprite> &getSpriteForType(TileType type);

/**
 * Gets a map, that maps Keyboard Keycodes to the TileType that should be selected by pressing said key.
 */
const std::map<SDL_Scancode, TileType> &getScancodeMap();

/**
 * Gets the name of the tile type
 * @param type A tile type to check
 * @return The name of the type
 */
char* &getName(TileType type);


/**
 * @param type A tile type to check
 * @return Whether this type of tile is solid
 */
bool isSolid(TileType type);

/**
 * @param type The type to check
 * @return if this tile type is special (Goal or Enemy Spawn)
 */
bool isSpecial(TileType type);

