//
// Created by skadic on 18.06.20.
//

#include "TilePosition.h"
#include "../../util/GlobalConsts.h"

TilePosition TilePosition::neighbor(Direction dir) {
    switch (dir) {
        case LEFT:
            return {static_cast<short>(x-1), y};
        case RIGHT:
            return {static_cast<short>(x+1), y};
        case UP:
            return {x, static_cast<short>(y - 1)};
        case DOWN:
            return {x, static_cast<short>(y + 1)};
    }
    return {0, 0};
}

bool TilePosition::operator==(TilePosition &a) {
    return x == a.x && y == a.y;
}

bool TilePosition::operator!=(TilePosition &a) {
    return !(*this == a);
}

bool TilePosition::operator<(const TilePosition &a) const {
    auto thisDistSquare = x * x + y * y;
    auto otherDistSquare = a.x * a.x + a.y * a.y;

    if(thisDistSquare < otherDistSquare) return true;
    if(thisDistSquare > otherDistSquare) return false;
    if(x < a.x) return true;
    if(x > a.x) return false;
    if(y < a.y) return true;
    return y > a.y;
}

Position TilePosition::toPosition() {
    return {static_cast<float>(x * TILE_SIZE), static_cast<float>(y * TILE_SIZE)};
}
