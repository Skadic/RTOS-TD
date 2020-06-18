#include "Sprites.h"
#include <iostream>

SpriteComponent::SpriteComponent(Sprite *sprite) : sprite{std::shared_ptr<Sprite>(sprite)} {}

std::shared_ptr<Sprite> &SpriteComponent::getSprite() {
    return sprite;
}

SpriteComponent::SpriteComponent(std::shared_ptr<Sprite> &sprite) : sprite{sprite} {}

void SpriteComponent::setSprite(std::shared_ptr<Sprite> &s) {
    this->sprite = s;
}

void SpriteComponent::setSprite(Sprite *s) {
    this->sprite = std::shared_ptr<Sprite>(s);
}


RectangleSprite::RectangleSprite(int width, int height, unsigned int color, bool filled) {
    this->width = width;
    this->height = height;
    this->color = color;
    this->filled = filled;
}

void RectangleSprite::draw(short x, short y, float scale) {
    if(filled) {
        tumDrawFilledBox(x, y, width * scale, height * scale, color);
    } else {
        tumDrawBox(x, y, width * scale, height * scale, color);
    }
}


TextureSprite::TextureSprite(std::string path) {
    path.insert(0, "../resources/");
    std::cout << path << std::endl;
    this->path = path;
}

void TextureSprite::draw(short x, short y, float scale) {
    if(!spriteHandle) {
        // Because we need a non-const char* and there's no better way to get that
        // We're loading this lazily, because images cannot be loaded when the SDL Renderer is not initialized yet
        auto chars = new char[path.size() + 1];
        std::copy(path.begin(), path.end(), chars);
        chars[path.size()] = '\0'; //0 Termination
        this->spriteHandle = tumDrawLoadImage(chars);
        tumDrawGetLoadedImageSize(this->spriteHandle, &this->width, &this->height);
    }
    tumDrawSetLoadedImageScale(this->spriteHandle, scale);
    if(tumDrawLoadedImage(this->spriteHandle, x, y)) {
        std::cout << "Failed to draw" << std::endl;
    };
}

// Unload the Image upon Destruction
TextureSprite::~TextureSprite() {
    std::cout << "Unloading image " << path << std::endl;
    tumDrawFreeLoadedImage(&this->spriteHandle);
}
