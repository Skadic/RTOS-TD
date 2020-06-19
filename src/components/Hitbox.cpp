
#include "Hitbox.h"
#include "../util/GlobalConsts.h"

#include <cmath>
#include <algorithm>
#include <iostream>


std::optional<glm::vec2> intersectsOther(Position pos, Hitbox hitbox, Position otherPos, Hitbox otherHitbox) {
    // Check for collision

    bool collided = pos.x < otherPos.x + otherHitbox.width &&
                    pos.x + hitbox.width > otherPos.x &&
                    pos.y < otherPos.y + otherHitbox.height &&
                    pos.y + hitbox.height > otherPos.y;

    if(!collided) {
        return std::optional<glm::vec2>{};
    }

    // Determine which distance would the
    const auto leftLen = pos.x - (otherPos.x + otherHitbox.width);
    const auto upLen = pos.y - (otherPos.y + otherHitbox.height);
    const auto rightLen = pos.x + hitbox.width - otherPos.x;
    const auto downLen = pos.y + hitbox.height - otherPos.y;

    const auto minDist = std::min({std::fabs(leftLen), std::fabs(upLen), std::fabs(rightLen), std::fabs(downLen)});
    
    if(minDist == std::fabs(leftLen)) {
        return std::make_optional(glm::vec2{leftLen, 0});
    } else if (minDist == std::fabs(upLen)) {
        return std::make_optional(glm::vec2{0, upLen});
    } else if (minDist == std::fabs(rightLen)) {
        return std::make_optional(glm::vec2{rightLen, 0});
    } else {
        return std::make_optional(glm::vec2{0, downLen});
    }
}

std::optional<glm::vec2> intersectsOther(TilePosition pos, Hitbox hitbox, Position otherPos, Hitbox otherHitbox) {
    Position newPos = Position{static_cast<float>(pos.x * TILE_SIZE), static_cast<float>(pos.y * TILE_SIZE)};
    return intersectsOther(newPos, hitbox, otherPos, otherHitbox);
}

bool hitboxInRange(Position rangePos, Range range, Position hitboxPos, Hitbox hitbox) {
    // Temporary variables to set edges for testing
    float testX = rangePos.x;
    float testY = rangePos.y;

    // Check which edge is closest to the center of the range
    if(rangePos.x < hitboxPos.x)                        testX = hitboxPos.x; // Check left edge
    else if (rangePos.x > hitboxPos.x + hitbox.width)   testX = hitboxPos.x + hitbox.width; //Right edge
    if(rangePos.y < hitboxPos.y)                        testY = hitboxPos.y; // Check top edge
    else if (rangePos.y > hitboxPos.y + hitbox.height)  testY = hitboxPos.y + hitbox.height; //Bottom edge

    float xDist = rangePos.x - testX;
    float yDist = rangePos.y - testY;
    float distSquared = xDist * xDist + yDist * yDist; // The distance squared. We'll compare this to the radius squared so we don't do an sqrt operation

    return distSquared <= range.radius * range.radius;
}