#pragma once

#include "../util/Direction.h"

struct TilePosition {
    short x;
    short y;

    TilePosition neighbor(Direction dir);

    bool operator==(TilePosition& a);
};