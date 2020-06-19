//
// Created by skadic on 28.05.20.
//

#include <cmath>
#include "Velocity.h"

void Velocity::normalize() {
    if (dx != 0 || dy != 0) {
        auto len = sqrt(dx * dx + dy * dy);
        dx /= len;
        dy /= len;
    }
}

void Velocity::towards(Position source, Position target, float speed) {
    dx = target.x - source.x;
    dy = target.y - source.y;
    normalize();
    dx *= speed;
    dy *= speed;
}
