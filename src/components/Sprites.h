#pragma once

#include <memory>
#include <string>


extern "C" {
    #include <TUM_Draw.h>
}


struct Sprite {
    int width;
    int height;

    virtual void draw(short x, short y) const = 0;

    virtual ~Sprite() = default;
};

class SpriteComponent {
    std::unique_ptr<Sprite> sprite;

public:
    explicit SpriteComponent(Sprite *sprite);
    std::unique_ptr<Sprite> &getSprite();
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