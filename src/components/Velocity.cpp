//
// Created by skadic on 28.05.20.
//

#include <cmath>
#include "Velocity.h"

float Velocity::normalize() {
    auto len = sqrt(dx * dx + dy * dy);
    dx /= len;
    dy /= len;
    return len;
}
