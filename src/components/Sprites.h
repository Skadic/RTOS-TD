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
    virtual void draw(short x, short y, float scale) = 0;
};

struct RectangleSprite : public Sprite {
    unsigned int color;
    bool filled;

    explicit RectangleSprite(int width, int height, unsigned int color, bool filled);

    void draw(short x, short y, float scale) override;
};


struct TextureSprite : public Sprite {
    std::string path;
    image_handle_t spriteHandle;

    explicit TextureSprite(std::string path);

    ~TextureSprite() override;

    void draw(short x, short y, float scale) override;
};

struct Texture2ColorSprite : public Sprite {
    std::string path;
    image_handle_t spriteHandle;
    unsigned int color1;
    unsigned int color2;

    explicit Texture2ColorSprite(std::string path, unsigned int color1, unsigned int color2);

    ~Texture2ColorSprite() override;

    void draw(short x, short y, float scale) override;
};


struct EmptySprite : public Sprite {
    void draw(short x, short y, float scale) override;
};

class SpriteComponent {
    std::shared_ptr<Sprite> sprite;

public:
    explicit SpriteComponent(std::shared_ptr<Sprite> &&sprite);
    explicit SpriteComponent(std::shared_ptr<Sprite> &sprite);
    explicit SpriteComponent(Sprite *sprite);
    std::shared_ptr<Sprite> &getSprite();
    void setSprite(std::shared_ptr<Sprite> &&s);
    void setSprite(Sprite *s);
};
