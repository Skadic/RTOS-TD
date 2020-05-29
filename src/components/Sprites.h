#pragma once

#include <memory>


extern "C" {
    #include <TUM_Draw.h>
}


struct Sprite {
    int width;
    int height;

    virtual void draw(short x, short y) = 0;
};

struct SpriteComponent {
    std::unique_ptr<Sprite> sprite;

    explicit SpriteComponent(std::unique_ptr<Sprite> &sprite);

    std::unique_ptr<Sprite>::pointer getSprite();
};


struct RectangleSprite : Sprite {
    unsigned int color;
    bool filled;

    explicit RectangleSprite(int width, int height, unsigned int color, bool filled);

    void draw(short x, short y) override;
};

struct TextureSprite : Sprite {
    std::string path;
    image_handle_t spriteHandle;

    explicit TextureSprite(std::string path);

    ~TextureSprite();

    void draw(short x, short y) override;
};