#include "Position.h"

#include <cmath>

float Position::distance(Position &other) {
    auto dist = other - *this;
    return std::sqrt(dist.x * dist.x + dist.y * dist.y);
}

Position Position::operator-(Position other) {
    return Position{x - other.x, y - other.y};
}

Position Position::operator+(Position other) {
    return Position{x + other.x, y + other.y};
}
