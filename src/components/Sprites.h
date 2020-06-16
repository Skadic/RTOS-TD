#pragma once

#include <memory>


extern "C" {
    #include <TUM_Draw.h>
}


struct Sprite {
    int width;
    int height;

    virtual void draw(short x, short y) const = 0;

    virtual ~Sprite() = default;
};

struct SpriteComponent {
    const Sprite *sprite;

    explicit SpriteComponent(const Sprite *sprite);

    const Sprite *getSprite() const;
};


struct RectangleSprite : Sprite {
    unsigned int color;
    bool filled;

    explicit RectangleSprite(int width, int height, unsigned int color, bool filled);

    void draw(short x, short y) const override;
};

struct TextureSprite : Sprite {
    std::string path;
    image_handle_t spriteHandle;

    explicit TextureSprite(std::string path);

    ~TextureSprite();

    void draw(short x, short y) const override;
};