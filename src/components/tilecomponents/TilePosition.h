#pragma once

#include "../../util/map/Direction.h"

struct TilePosition {
    short x;
    short y;

    TilePosition neighbor(Direction dir);

    bool operator==(TilePosition& a);
};