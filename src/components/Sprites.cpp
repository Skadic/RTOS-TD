#include "Sprites.h"

RectangleSprite::RectangleSprite(unsigned short width, unsigned short height, unsigned int color, bool filled) {
    this->width = width;
    this->height = height;
    this->color = color;
    this->filled = filled;
}

void RectangleSprite::draw(short x, short y) {
    if(filled) {
        tumDrawFilledBox(x, y, width, height, color);
    } else {
        tumDrawBox(x, y, width, height, color);
    }
}