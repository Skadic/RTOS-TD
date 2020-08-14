#pragma once

#include "../../util/map/Direction.h"
#include "../Position.h"

/**
 * A component that represents a tile's position in the grid of the map.
 * This is not interchangable with Position!
 */
struct TilePosition {
    /**
     * The x coordinate of this tile
     */
    short x;

    /**
     * The y coordinate of this tile
     */
    short y;

    /**
     * Returns the tile position that is 1 step in the given direction
     * E.g. if the Position was `(0, 1)` and `dir = RIGHT`, then the result is `(1, 1)`
     * @param dir The direction to get the neighbor of
     * @return The tile position that is 1 step into the given direction.
     */
    TilePosition neighbor(Direction dir);

    /**
     * Component-wise comparison
     */
    bool operator==(TilePosition& a);

    /**
     * Component-wise comparison
     */
    bool operator!=(TilePosition& a);

    /**
     * An artificial ordering only used for things like ordered sets and maps
     */
    bool operator<(const TilePosition& a) const;

    /**
     * Converts this tile position to an actual position in the game world by using the TILE_SIZE constant
     * @return This tile's actual position in the game world
     */
    Position toPosition();
};