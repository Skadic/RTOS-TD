#pragma once

enum Direction {
    UP,
    RIGHT,
    DOWN,
    LEFT,
    LAST_DIRECTION
};

Direction nextRight(Direction dir);
Direction nextLeft(Direction dir);


