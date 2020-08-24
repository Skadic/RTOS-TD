//
// Created by skadic on 18.06.20.
//

#include "Renderer.h"
#include "../components/Hitbox.h"

#include <TUM_Draw.h>


bool Renderer::boxOutOfBounds(short x, short y, short width, short height) {
    return !(getScreenX(x) < SCREEN_WIDTH && getScreenY(y) < SCREEN_HEIGHT &&
           getScreenX(x + width) > 0 && getScreenY(y + height) > 0);
}

bool Renderer::circleOutOfBounds(short x, short y, short radius) {
    float transX = getScreenX(x);
    float transY = getScreenY(y);
    float scaledRadius = radius * scale;

    return !intersectHitboxRange({transX, transY}, {scaledRadius}, {0, 0}, {SCREEN_WIDTH, SCREEN_HEIGHT});
}

bool Renderer::lineOutOfBounds(short x1, short y1, short x2, short y2) {
    float transX1 = getScreenX(x1);
    float transY1 = getScreenY(y1);
    float transX2 = getScreenX(x2);
    float transY2 = getScreenY(y2);

    return !intersectLineRect({transX1, transY1},{transX2, transY2},{0, 0},{SCREEN_WIDTH, SCREEN_HEIGHT});
}

Renderer::Renderer(short xOffset, short yOffset, float scale) : xOffset{xOffset}, yOffset{yOffset}, scale{scale} {}

void Renderer::drawBox(short x, short y, short width, short height, unsigned int color, bool filled) {
    // Check if the box is visible on screen and if so, draw it
    if(!boxOutOfBounds(x, y, width, height)) {
        if(filled) {
            tumDrawFilledBox(getScreenX(x), getScreenY(y), width * scale, height * scale, color);
        } else {
            tumDrawBox(getScreenX(x), getScreenY(y), width * scale, height * scale, color);
        }
    }
}

void Renderer::drawSprite(Sprite &sprite, short x, short y) {
    // Only draw the sprite if any part of it is visible on screen
    if(!boxOutOfBounds(x, y, sprite.width, sprite.height)) {
        sprite.draw(getScreenX(x), getScreenY(y), this->scale);
    }
}

void Renderer::setScale(float s) {
    this->scale = s;
}

void Renderer::setOffset(short x, short y) {
    this->xOffset = x;
    this->yOffset = y;
}

void Renderer::setXOffset(short x) {
    this->xOffset = x;
}

void Renderer::setYOffset(short y) {
    this->yOffset = y;
}

short Renderer::getScreenX(short gameX) {
    return (gameX + xOffset) * scale;
}

short Renderer::getScreenY(short gameY) {
    return (gameY + yOffset) * scale;
}

short Renderer::getGameX(short screenX) {
    return (screenX / scale) - xOffset;
}

short Renderer::getGameY(short screenY) {
    return (screenY / scale) - yOffset;
}

void Renderer::addOffset(short x, short y) {
    this->xOffset += x;
    this->yOffset += y;
}

void Renderer::addXOffset(short x) {
    this->xOffset += x;
}

void Renderer::addYOffset(short y) {
    this->yOffset += y;
}

float Renderer::getScale() {
    return scale;
}

short Renderer::getYOffset() {
    return yOffset;
}

short Renderer::getXOffset() {
    return xOffset;
}

void Renderer::drawLine(short x1, short y1, short x2, short y2, unsigned char thickness, unsigned int color) {
    // Check if the line is in bounds and draw it if so
    if(!lineOutOfBounds(x1, y1, x2, y2)) {
        float transX1 = getScreenX(x1);
        float transY1 = getScreenY(y1);
        float transX2 = getScreenX(x2);
        float transY2 = getScreenY(y2);
        tumDrawLine(transX1, transY1, transX2, transY2, thickness, color);
    }
}

void Renderer::drawCircle(short x, short y, short radius, unsigned int color, bool filled) {
    // Check if the circle is in bounds and draw it if so
    if (!circleOutOfBounds(x, y, radius)) {
        float transX = getScreenX(x);
        float transY = getScreenY(y);
        float scaledRadius = radius * scale;

        if (filled) {
            tumDrawCircle(transX, transY, scaledRadius, color);
        } else {
            tumDrawCircleClear(transX, transY, scaledRadius, color);
        }
    }
}

void Renderer::drawPie(short x, short y, short radius, short start, short end, unsigned int color, bool filled) {

    // Check if the pie is in bounds and draw it if so
    if (!circleOutOfBounds(x, y, radius)) {
        float transX = getScreenX(x);
        float transY = getScreenY(y);
        float scaledRadius = radius * scale;

        if (filled) {
            tumDrawPieFilled(transX, transY, scaledRadius, start, end, color);
        } else {
            tumDrawPie(transX, transY, scaledRadius, start, end, color);
        }
    }
}

void Renderer::drawText(char *text, short x, short y, unsigned int color) {
    tumDrawText(text, getScreenX(x), getScreenY(y), color);
}




