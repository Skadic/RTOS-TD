#pragma once

struct Position {
    float x;
    float y;

    float distance(Position &other);

    Position operator-(Position other);
    Position operator+(Position other);
};
