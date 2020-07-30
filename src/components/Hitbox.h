#pragma once

#include "Position.h"
#include "tilecomponents/TilePosition.h"
#include <glm.hpp>
#include <optional>

struct Hitbox {
    float width;
    float height;
    bool solid;

    Hitbox(float width, float height, bool solid = true) : width{width}, height{height}, solid{solid} {}
};

// A circular Range used for towers
struct Range {
    float radius;
};

bool intersectPointHitbox(Position pos, Position hitboxPos, Hitbox hitbox);

std::optional<glm::vec2> intersectHitbox(Position pos, Hitbox hitbox, Position otherPos, Hitbox otherHitbox);

std::optional<glm::vec2> intersectHitbox(TilePosition pos, Hitbox hitbox, Position otherPos, Hitbox otherHitbox);

bool intersectHitboxRange(Position rangePos, Range range, Position hitboxPos, Hitbox hitbox);

bool intersectLine(short x1, short y1, short x2, short y2);

bool intersectLineRect(Position p1, Position p2, Position rectPos, Hitbox hitbox);