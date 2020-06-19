//
// Created by skadic on 28.05.20.
//

#include <cmath>
#include "Velocity.h"

// Computes 1/sqrt(x) approximately. Fast inverse square root algorithm
float fastInverseSquareRoot(float number) {
    long i;
    float x2, y;
    const float threehalfs = 1.5F;

    x2 = number * 0.5F;
    y  = number;
    i  = * ( long * ) &y;
    i  = 0x5f3759df - ( i >> 1 );
    y  = * ( float * ) &i;
    y  = y * ( threehalfs - ( x2 * y * y ) );
    y  = y * ( threehalfs - ( x2 * y * y ) );

    return y;
}

void Velocity::normalize() {
    if (dx != 0 || dy != 0) {
        auto len = fastInverseSquareRoot(dx * dx + dy * dy);
        dx *= len;
        dy *= len;
    }
}
