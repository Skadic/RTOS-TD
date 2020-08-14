#pragma once

/**
 * Represents the 4 cardinal directions
 */
enum Direction {
    UP,
    RIGHT,
    DOWN,
    LEFT,
    /**
     * Only used for array/vector indexing purposes
     */
    LAST_DIRECTION
};

/**
 * The next direction in a clockwise order
 */
Direction nextRight(Direction dir);

/**
 * The next direction in a counter-clockwise order
 */
Direction nextLeft(Direction dir);


