#include "Sprites.h"
#include <iostream>

SpriteComponent::SpriteComponent(const Sprite *sprite) : sprite{sprite} {}

const Sprite *SpriteComponent::getSprite() const {
    return sprite;
}

RectangleSprite::RectangleSprite(int width, int height, unsigned int color, bool filled) {
    this->width = width;
    this->height = height;
    this->color = color;
    this->filled = filled;
}

void RectangleSprite::draw(short x, short y) const {
    if(filled) {
        tumDrawFilledBox(x, y, width, height, color);
    } else {
        tumDrawBox(x, y, width, height, color);
    }
}


TextureSprite::TextureSprite(std::string path) {
    path.insert(0, "../resources/");
    // Because we need a non-const char* and there's no better way to get that
    auto chars = new char[path.size() + 1];
    std::copy(path.begin(), path.end(), chars);
    chars[path.size()] = '\0'; //0 Termination

    this->path = path;
    this->spriteHandle = tumDrawLoadImage(chars);
    tumDrawGetLoadedImageSize(this->spriteHandle, &this->width, &this->height);
}

void TextureSprite::draw(short x, short y) const {
    if(tumDrawLoadedImage(this->spriteHandle, x, y)) {
        std::cout << "Failed to draw" << std::endl;
    };
}

// Unload the Image upon Destruction
TextureSprite::~TextureSprite() {
    std::cout << "Unloading image " << path << std::endl;
    std::cout.flush();
    tumDrawFreeLoadedImage(&this->spriteHandle);
}
