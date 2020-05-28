
#include "Hitbox.h"


bool intersectsOther(Position pos, Hitbox hitbox, Position otherPos, Hitbox otherHitbox) {
    return (((pos.x < otherPos.x) && (otherPos.x < pos.x + hitbox.width)) // Check whether the left edge of the other hitbox is within the left and right edge of this hitbox
        || ((pos.x < otherPos.x + otherHitbox.width) && (otherPos.x + otherHitbox.width < pos.x + hitbox.width))) // Check whether the right edge of the other hitbox is within the left and right edge of this hitbox
        && (((pos.y < otherPos.y) && (otherPos.y < pos.y + hitbox.height)) // Check whether the top edge of the other hitbox is within the top and bottom edge of this hitbox
        || ((pos.y < otherPos.y + otherHitbox.height) && (otherPos.y + otherHitbox.width < pos.x + hitbox.width))); // Check whether the bottom edge of the other hitbox is within the top and bottom edge of this hitbox
}