#pragma once

#include "../components/Sprites.h"

class Renderer {
    short xOffset;
    short yOffset;
    float scale;
public:

    explicit Renderer(short xOffset = 0, short yOffset = 0, float scale = 1);

    void drawSprite(Sprite &sprite, short x, short y);

    void setScale(float s);
    void setOffset(short x, short y);
    void setXOffset(short x);
    void setYOffset(short y);

    void addOffset(short x, short y);
    void addXOffset(short x);
    void addYOffset(short y);

    short getXOffset();
    short getYOffset();
    float getScale();


private:
    short getTransformedX(short x);
    short getTransformedY(short y);
};
