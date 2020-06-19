
#include "Hitbox.h"


bool intersectsOther(Position pos, Hitbox hitbox, Position otherPos, Hitbox otherHitbox) {
    return (((pos.x < otherPos.x) && (otherPos.x < pos.x + hitbox.width)) // Check whether the left edge of the other hitbox is within the left and right edge of this hitbox
        || ((pos.x < otherPos.x + otherHitbox.width) && (otherPos.x + otherHitbox.width < pos.x + hitbox.width))) // Check whether the right edge of the other hitbox is within the left and right edge of this hitbox
        && (((pos.y < otherPos.y) && (otherPos.y < pos.y + hitbox.height)) // Check whether the top edge of the other hitbox is within the top and bottom edge of this hitbox
        || ((pos.y < otherPos.y + otherHitbox.height) && (otherPos.y + otherHitbox.width < pos.x + hitbox.width))); // Check whether the bottom edge of the other hitbox is within the top and bottom edge of this hitbox
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