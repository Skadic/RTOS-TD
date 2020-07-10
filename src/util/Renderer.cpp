//
// Created by skadic on 18.06.20.
//

#include "Renderer.h"
#include "../components/Hitbox.h"

#include <TUM_Draw.h>
#include <iostream>


Renderer::Renderer(short xOffset, short yOffset, float scale) : xOffset{xOffset}, yOffset{yOffset}, scale{scale} {}

void Renderer::drawBox(short x, short y, short width, short height, unsigned int color, bool filled) {
    if(
            getTransformedX(x) < SCREEN_WIDTH &&
            getTransformedY(y) < SCREEN_HEIGHT &&
            getTransformedX(x + width) > 0 &&
            getTransformedY(y + height) > 0) {
        if(filled) {
            tumDrawFilledBox(getTransformedX(x), getTransformedY(y), width * scale, height * scale, color);
        } else {
            tumDrawBox(getTransformedX(x), getTransformedY(y), width * scale, height * scale, color);
        }
    }
}

void Renderer::drawSprite(Sprite &sprite, short x, short y) {
    // Only draw the sprite if any part of it is visible on screen
    if(
            getTransformedX(x) < SCREEN_WIDTH &&
            getTransformedY(y) < SCREEN_HEIGHT &&
            getTransformedX(x + sprite.width) > 0 &&
            getTransformedY(y + sprite.height) > 0) {
        sprite.draw(getTransformedX(x), getTransformedY(y), this->scale);
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

short Renderer::getTransformedX(short x) {
    return (x + xOffset) * scale;
}

short Renderer::getTransformedY(short y) {
    return (y + yOffset) * scale;
}

short Renderer::reverseTransformX(short transformedX) {
    return (transformedX / scale) - xOffset;
}

short Renderer::reverseTransformY(short transformedY) {
    return (transformedY / scale) - yOffset;
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
    float transX1 = getTransformedX(x1);
    float transY1 = getTransformedY(y1);
    float transX2 = getTransformedX(x2);
    float transY2 = getTransformedY(y2);
    if(intersectLineRect(
            {transX1, transY1},
            {transX2, transY2},
            {0, 0},
            {SCREEN_WIDTH, SCREEN_HEIGHT})) {
        tumDrawLine(transX1, transY1, transX2, transY2, thickness, color);
    }
}

void Renderer::drawCircle(short x, short y, short radius, unsigned int color, bool filled) {

    float transX = getTransformedX(x);
    float transY = getTransformedY(y);
    float scaledRadius = radius * scale;

    if (intersectHitboxRange({transX, transY}, {scaledRadius}, {0, 0}, {SCREEN_WIDTH, SCREEN_HEIGHT})) {
        if (filled) {
            tumDrawCircle(transX, transY, scaledRadius, color);
        } else {
            tumDrawCircleClear(transX, transY, scaledRadius, color);
        }
    }
}

void Renderer::drawPie(short x, short y, short radius, short start, short end, unsigned int color, bool filled) {
    float transX = getTransformedX(x);
    float transY = getTransformedY(y);
    float scaledRadius = radius * scale;

    if (intersectHitboxRange({transX, transY}, {scaledRadius}, {0, 0}, {SCREEN_WIDTH, SCREEN_HEIGHT})) {
        if (filled) {
            tumDrawPieFilled(transX, transY, scaledRadius, start, end, color);
        } else {
            tumDrawPie(transX, transY, scaledRadius, start, end, color);
        }
    }
}




