
#include "Hitbox.h"
#include "../util/GlobalConsts.h"

#include <cmath>
#include <algorithm>
#include <iostream>


std::optional<glm::vec2> intersectHitbox(Position pos, Hitbox hitbox, Position otherPos, Hitbox otherHitbox) {
    // Check for collision

    bool collided = pos.x < otherPos.x + otherHitbox.width &&
                    pos.x + hitbox.width > otherPos.x &&
                    pos.y < otherPos.y + otherHitbox.height &&
                    pos.y + hitbox.height > otherPos.y;

    if(!collided) {
        return std::optional<glm::vec2>{};
    }

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

std::optional<glm::vec2> intersectHitbox(TilePosition pos, Hitbox hitbox, Position otherPos, Hitbox otherHitbox) {
    Position newPos = Position{static_cast<float>(pos.x * TILE_SIZE), static_cast<float>(pos.y * TILE_SIZE)};
    return intersectHitbox(newPos, hitbox, otherPos, otherHitbox);
}

bool intersectHitboxRange(Position rangePos, Range range, Position hitboxPos, Hitbox hitbox) {
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
    float dist = std::sqrt(xDist * xDist + yDist * yDist); // The distance squared. We'll compare this to the radius squared so we don't do an sqrt operation

    return dist <= range.radius;
}

bool lineIntersect(Position p1, Position p2, Position p3, Position p4) {
    // calculate the direction of the lines
    float uA = ((p4.x-p3.x)*(p1.y-p3.y) - (p4.y-p3.y)*(p1.x-p3.x)) / ((p4.y-p3.y)*(p2.x-p1.x) - (p4.x-p3.x)*(p2.y-p1.y));
    float uB = ((p2.x-p1.x)*(p1.y-p3.y) - (p2.y-p1.y)*(p1.x-p3.x)) / ((p4.y-p3.y)*(p2.x-p1.x) - (p4.x-p3.x)*(p2.y-p1.y));

    // if uA and uB are between 0-1, lines are colliding
    return uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1;
}

bool intersectLineRect(Position p1, Position p2, Position rectPos, Hitbox hitbox) {
    auto left = lineIntersect(p1, p2, rectPos, {rectPos.x, rectPos.y + hitbox.height});
    auto top = lineIntersect(p1, p2, rectPos, {rectPos.x + hitbox.width, rectPos.y});
    auto bottom = lineIntersect(p1, p2, {rectPos.x, rectPos.y + hitbox.height}, {rectPos.x + hitbox.width, rectPos.y + hitbox.height});
    auto right = lineIntersect(p1, p2, {rectPos.x + hitbox.width, rectPos.y}, {rectPos.x + hitbox.width, rectPos.y + hitbox.height});

    return left || top || bottom || right || intersectPointHitbox(p1, rectPos, hitbox) || intersectPointHitbox(p2, rectPos, hitbox);
}

bool intersectPointHitbox(Position pos, Position hitboxPos, Hitbox hitbox) {
    return pos.x >= hitboxPos.x && pos.x <= hitboxPos.x + hitbox.width
    && pos.y >= hitboxPos.y && pos.y <= hitboxPos.y + hitbox.height;
}


