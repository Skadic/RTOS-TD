#pragma once

#include "Position.h"
#include "tilecomponents/TilePosition.h"
#include <glm.hpp>
#include <optional>

/**
 * A component representing a rectangular axis-aligned hitbox
 */
struct Hitbox {
    /**
     * The width of the hitbox
     */
    float width;

    /**
     * The height of the hitbox
     */
    float height;

    /**
     * Whether the hitbox is solid or not
     * If it is solid, moving entities cannot pass through it
     */
    bool solid;

    Hitbox(float width, float height, bool solid = false) : width{width}, height{height}, solid{solid} {}
};

/**
 * A circular Range used for towers
 */
struct Range {
    float radius;
};

/**
 * Checks whether a Point is in a hitbox
 * @param pos The point to check
 * @param hitboxPos The position of the hitbox
 * @param hitbox The hitbox
 * @return true, if the point is within the hitbox, false otherwise
 */
bool intersectPointHitbox(Position pos, Position hitboxPos, Hitbox hitbox);

/**
 * Checks whether 2 hitboxes intersect and calculates a displacement vector
 * @param pos The position of the first hitbox
 * @param hitbox The first hitbox
 * @param otherPos The position of the second hitbox
 * @param otherHitbox The second hitbox
 * @return std::nullopt if there is no intersection, otherwise returns a displacement vector wrapped in an optional.
 * This vector is the shortest vector that you must move the second hitbox to resolve the collision
 */
std::optional<glm::vec2> intersectHitbox(Position pos, Hitbox hitbox, Position otherPos, Hitbox otherHitbox);

/**
 * Checks whether 2 hitboxes intersect and calculates a displacement vector. Here, the first hitbox has a tile position
 * instead of a position
 * @param pos The position of the first hitbox
 * @param hitbox The first hitbox
 * @param otherPos The position of the second hitbox
 * @param otherHitbox The second hitbox
 * @return std::nullopt if there is no intersection, otherwise returns a displacement vector wrapped in an optional.
 * This vector is the shortest vector that you must move the second hitbox to resolve the collision
 */
std::optional<glm::vec2> intersectHitbox(TilePosition pos, Hitbox hitbox, Position otherPos, Hitbox otherHitbox);

/**
 * Checks whether a hitbox and a range intersect
 * @param rangePos The position of the range
 * @param range The range
 * @param hitboxPos The position of the hitbox
 * @param hitbox The hitbox
 * @return true if the range and the hitbox intersect, false otherwise
 */
bool intersectHitboxRange(Position rangePos, Range range, Position hitboxPos, Hitbox hitbox);

/**
 * Checks whether a line and a hitbox intersect
 * @param p1 The start point of the line
 * @param p2 The end point of the line
 * @param rectPos The position of the hitbox
 * @param hitbox The hitbox
 * @return true if the line and the hitbox intersect, false otherwise
 */
bool intersectLineRect(Position p1, Position p2, Position rectPos, Hitbox hitbox);