#pragma once

#include "../../util/map/Direction.h"
#include "../Position.h"

struct TilePosition {
    short x;
    short y;

    TilePosition neighbor(Direction dir);

    bool operator==(TilePosition& a);
    bool operator!=(TilePosition& a);
    bool operator<(const TilePosition& a) const;

    Position toPosition();
};