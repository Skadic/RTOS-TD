#pragma once

#include "Position.h"
#include "tilecomponents/TilePosition.h"
#include <glm.hpp>
#include <optional>

struct Hitbox {
    float width;
    float height;
};

// A circular Range used for towers
struct Range {
    float radius;
};

std::optional<glm::vec2> intersectsOther(Position pos, Hitbox hitbox, Position otherPos, Hitbox otherHitbox);

std::optional<glm::vec2> intersectsOther(TilePosition pos, Hitbox hitbox, Position otherPos, Hitbox otherHitbox);

bool hitboxInRange(Position rangePos, Range range, Position hitboxPos, Hitbox hitbox);
