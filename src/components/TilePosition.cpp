//
// Created by skadic on 18.06.20.
//

#include "TilePosition.h"

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
