#pragma once

extern "C" {
    #include <TUM_Draw.h>
}

struct Sprite {
    virtual void draw(short x, short y) = 0;
};

struct RectangleSprite : Sprite {
    unsigned short width;
    unsigned short height;
    unsigned int color;
    bool filled;

    RectangleSprite(unsigned short width, unsigned short height, unsigned int color, bool filled);

    void draw(short x, short y) override;
};