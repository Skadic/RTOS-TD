#pragma once

/**
 * A component representing an entity's position on the game world
 */
struct Position {
    /**
     * The x position
     */
    float x;

    /**
     * The y position
     */
    float y;

    /**
     * Calculates the euclidian distance between this position and another
     * @param other The other position
     * @return The euclidian distance between the two positions
     */
    float distance(Position &other);

    /**
     * Calculates the component-wise difference of 2 positions
     * @param other The other position
     * @return the component-wise difference of 2 positions
     */
    Position operator-(Position other);

    /**
     * Calculates the component-wise sum of 2 positions
     * @param other The other position
     * @return the component-wise sum of 2 positions
     */
    Position operator+(Position other);
};
