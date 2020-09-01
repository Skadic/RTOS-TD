#pragma once

#include <map>
#include "../../components/Sprites.h"
#include <SDL_scancode.h>

/**
 * All the types of tiles that a tile can hold. The values of these enum constants are which are used in the json
 */
enum TileType {
    EMPTY = 0,
    NON_BUILDABLE = 1,
    BUILDABLE_SOLID = 2,
    NON_BUILDABLE_SOLID = 3,
    NEXUS = 4,
    ENEMY_SPAWN = 5,
    WALL = 6,
    TOWER_PROJECTILE = 7,
    TOWER_LASER = 8,
    TOWER_AOE = 9,
    LAST_TILE_TYPE = 10 // Constant, only used to initialize Arrays etc. Not to be used. Add new enum constants only BEFORE this one
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
 * @return if this tile type is special (Goal, Enemy Spawn, non-buildable tile etc.)
 */
bool isSpecial(TileType type);


/**
 * Returns the cost to upgrade the given tower type from level "level - 1" to "level"
 * @param type The tower type to upgrade
 * @param level The level for the tower to reach
 * @return The cost
 */
int levelUpCost(TileType type, int level);

/**
 * Returns the total cost of building a tower of the given type with the given level
 * @param type The type of tower to build
 * @param level The desired level of the tower
 * @return The cost
 */
int costAtLevel(TileType type, int level);

