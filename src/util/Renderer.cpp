//
// Created by skadic on 18.06.20.
//

#include "Renderer.h"

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


