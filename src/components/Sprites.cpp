#include "Sprites.h"
#include <iostream>
#include <random>


inline const auto TEXTURE_PATH = "../resources/textures/";

SpriteComponent::SpriteComponent(Sprite *sprite) : sprite{std::shared_ptr<Sprite>(sprite)} {}

std::shared_ptr<Sprite> &SpriteComponent::getSprite() {
    return sprite;
}

SpriteComponent::SpriteComponent(std::shared_ptr<Sprite> &sprite) : sprite{sprite} {}

void SpriteComponent::setSprite(std::shared_ptr<Sprite> &&s) {
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
    if (filled) {
        tumDrawFilledBox(x, y, width * scale, height * scale, color);
    } else {
        tumDrawBox(x, y, width * scale, height * scale, color);
    }
}


TextureSprite::TextureSprite(std::string path) : spriteHandle{nullptr} {
    path.insert(0, TEXTURE_PATH);
    this->path = path;
}

void TextureSprite::draw(short x, short y, float scale) {
    // If the sprite is not loaded yet, load it
    if(!spriteHandle) {
        std::cout << "Loading image:" << path << std::endl;
        // We're loading this lazily, because images cannot be loaded when the SDL Renderer is not initialized yet
        this->spriteHandle = tumDrawLoadImage(const_cast<char*>(path.c_str()));
        tumDrawGetLoadedImageSize(this->spriteHandle, &this->width, &this->height);
    }
    tumDrawSetLoadedImageScale(this->spriteHandle, scale);
    if(tumDrawLoadedImage(this->spriteHandle, x, y)) {
        std::cout << "Failed to draw" << std::endl;
    };
}

TextureSprite::~TextureSprite() {
    std::cout << "Unloading image " << path << std::endl;
    // If the Image is not loaded yet, it does not need to be freed either
    if(isLoaded()) tumDrawFreeLoadedImage(&this->spriteHandle);
}

bool TextureSprite::isLoaded() const {
    return spriteHandle != nullptr;
}

Texture2ColorSprite::Texture2ColorSprite(std::string path, unsigned int colorWhite, unsigned int colorBlack) :
    TextureSprite(path), colorWhite{colorWhite}, colorBlack{colorBlack} {}


void Texture2ColorSprite::draw(short x, short y, float scale) {
    // If the sprite is not loaded yet, load it
    if(!spriteHandle) {
        // We're loading this lazily, because images cannot be loaded when the SDL Renderer is not initialized yet
        this->spriteHandle = tumDrawLoad2ColorImage(const_cast<char*>(path.c_str()), colorWhite, colorBlack);
        tumDrawGetLoadedImageSize(this->spriteHandle, &this->width, &this->height);
    }
    tumDrawSetLoadedImageScale(this->spriteHandle, scale);
    if(tumDrawLoadedImage(this->spriteHandle, x, y)) {
        std::cout << "Failed to draw" << std::endl;
    };
}
