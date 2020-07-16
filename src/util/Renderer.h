#pragma once

#include "../components/Sprites.h"

class Renderer {
    short xOffset;
    short yOffset;
    float scale;
public:

    explicit Renderer(short xOffset = 0, short yOffset = 0, float scale = 1);

    void drawLine(short x1, short y1, short x2, short y2, unsigned char thickness, unsigned int color);
    void drawBox(short x, short y, short width, short height, unsigned int color, bool filled);
    void drawCircle(short x, short y, short radius, unsigned int color, bool filled);
    void drawPie(short x, short y, short radius, short start, short end, unsigned int color, bool filled);
    void drawSprite(Sprite &sprite, short x, short y);
    void drawText(char* text, short x, short y, unsigned int color);

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

    short getScreenX(short gameX);
    short getScreenY(short gameY);

    short getGameX(short screenX);
    short getGameY(short screenY);
};
