#pragma once

#include "Position.h"

struct Hitbox {
    float width;
    float height;
};

// A circular Range used for towers
struct Range {
    float radius;
};

bool intersectsOther(Position &pos, Hitbox &hitbox, Position &otherPos, Hitbox &otherHitbox);

bool hitboxInRange(Position &rangePos, Range &range, Position &hitboxPos, Hitbox &hitbox);