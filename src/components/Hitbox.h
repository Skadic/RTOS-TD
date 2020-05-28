#pragma once

#include "Position.h"

struct Hitbox {
    float width;
    float height;
};

bool intersectsOther(Position pos, Hitbox hitbox, Position otherPos, Hitbox otherHitbox);