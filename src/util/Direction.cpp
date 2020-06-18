//
// Created by skadic on 18.06.20.
//

#include "Direction.h"


Direction nextRight(Direction dir) {
    switch (dir) {
        case UP:
            return RIGHT;
        case RIGHT:
            return DOWN;
        case DOWN:
            return LEFT;
        case LEFT:
            return UP;
    }
}


Direction nextLeft(Direction dir) {
    switch (dir) {
        case UP:
            return LEFT;
        case LEFT:
            return DOWN;
        case DOWN:
            return RIGHT;
        case RIGHT:
            return UP;
    }
}