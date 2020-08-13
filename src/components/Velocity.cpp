//
// Created by skadic on 28.05.20.
//

#include <cmath>
#include <algorithm>
#include "Velocity.h"

void Velocity::normalize() {
    if (dx != 0 || dy != 0) {
        auto len = sqrt(dx * dx + dy * dy);
        dx /= len;
        dy /= len;
    }
}

void Velocity::towards(Position source, Position target) {
    towards(source, target, maxSpeed);
}

void Velocity::towards(Position source, Position target, float speed) {
    dx = target.x - source.x;
    dy = target.y - source.y;
    normalize();
    dx *= std::min(speed, currentMaxSpeed);
    dy *= std::min(speed, currentMaxSpeed);
}

Velocity::Velocity(float dx, float dy, float maxSpeed) : dx{dx}, dy{dy}, maxSpeed{maxSpeed}, currentMaxSpeed{maxSpeed} {}

const float Velocity::getMaxSpeed() const {
    return maxSpeed;
}

float Velocity::getCurrentMaxSpeed() const {
    return currentMaxSpeed;
}

void Velocity::setCurrentMaxSpeed(float speed) {
    currentMaxSpeed = std::clamp(speed, 0.0f, maxSpeed);
}

void Velocity::resetCurrentMaxSpeed() {
    currentMaxSpeed = maxSpeed;
}
