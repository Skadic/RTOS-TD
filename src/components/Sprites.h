#pragma once

#include <memory>
#include <string>


extern "C" {
    #include <TUM_Draw.h>
}


struct Sprite {
    friend class Renderer;
    int width;
    int height;

    virtual ~Sprite() = default;

private:
    virtual void draw(short x, short y, float scale) const = 0;
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

    void draw(short x, short y, float scale) const override;
};

struct TextureSprite : Sprite {
    std::string path;
    image_handle_t spriteHandle;

    explicit TextureSprite(std::string path);

    ~TextureSprite() override;

    void draw(short x, short y, float scale) const override;
};