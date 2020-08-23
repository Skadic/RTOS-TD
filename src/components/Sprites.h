#pragma once

#include <memory>
#include <string>


extern "C" {
    #include <TUM_Draw.h>
}


/**
 * A general sprite class that can be drawn to the screen
 */
struct Sprite {
    // Only the Renderer class is allowed to draw spites
    friend class Renderer;
    /**
     * The sprite's width
     */
    int width;

    /**
     * The sprite's height
     */
    int height;

    /**
     * No special deconstruction is required for the generic sprite
     * There might be some sprites though that require special cleanup
     */
    virtual ~Sprite() = default;

private:
    /**
     * Draws the Sprite to the screen at the specified scale
     * @param x The x position where the sprite should be rendered
     * @param y The y position where the sprite should be rendered
     * @param scale The scale of the sprite
     */
    virtual void draw(short x, short y, float scale) = 0;
};

/**
 * A sprite which is a simple rectangle (filled or not)
 */
struct RectangleSprite : public Sprite {
    /**
     * The rectangle's color
     */
    unsigned int color;

    /**
     * Whether the rectangle should be filled or not
     */
    bool filled;

    /**
     * Constructs a rectangle sprite with the specified parameters
     * @param width The rectangle's width
     * @param height The rectangle's height
     * @param color The rectangle's color
     * @param filled Whether the rectangle should be filled or not
     */
    explicit RectangleSprite(int width, int height, unsigned int color, bool filled);

    void draw(short x, short y, float scale) override;
};

/**
 * A sprite which is represented by an image
 */
struct TextureSprite : public Sprite {
    /**
     * The path to the image
     */
    std::string path;

    /**
     * The image handle. This is loaded on the first draw call. This cannot be loaded statically,
     * as the SDL Renderer will not have been initialized by that point
     */
    image_handle_t spriteHandle;

    /**
     * Constructs a Texture sprite from the image at the given path
     * @param path The path to the image. It is assumed that the image is in "resources/textures",
     * so only the path relative to this one must be provided
     */
    explicit TextureSprite(std::string path);

    /**
     * This needs to be overridden, because the image needs to be freed
     */
    ~TextureSprite() override;

    /**
     * Checks whether the corresponding image has been loaded yet. This is not always the case as the image is only loaded
     * at the first draw call
     * @return True, if the image is already loaded, false otherwise
     */
    bool isLoaded() const;

    void draw(short x, short y, float scale) override;
};

/**
 * A sprite which takes an image and replaces white and black with 2 given colors.
 * This sprite does not support transparency!
 */
struct Texture2ColorSprite : public TextureSprite {

    /**
     * The color which white will be replaced by
     */
    unsigned int colorWhite;

    /**
     * The color which black will be replaced by
     */
    unsigned int colorBlack;


    /**
     * Constructs a Texture 2 color sprite from the image at the given path and 2 given colors
     * @param path The path to the image. It is assumed that the image is in "resources/textures",
     * so only the path relative to this one must be provided
     * @param colorWhite This color will replace all white pixels in the image
     * @param colorBlack This color will replace all black pixels in the image
     */
    explicit Texture2ColorSprite(std::string path, unsigned int colorWhite, unsigned int colorBlack);

    void draw(short x, short y, float scale) override;
};


/**
 * An empty sprite, which does not render anything
 */
struct EmptySprite : public Sprite {
    void draw(short x, short y, float scale) override {};
};

/**
 * A wrapper for sprites, so they can be added to entities as a component
 */
class SpriteComponent {
    /**
     * The underlying sprite
     * Multiple entities might use the same sprite, so these are shared pointers
     * And once this sprite is not used anymore, it is cleaned up automatically
     */
    std::shared_ptr<Sprite> sprite;

public:

    /**
     * Creates a sprite component from a previously existing shared pointer
     * @param sprite The shared pointer to the sprite that is to be used
     */
    explicit SpriteComponent(std::shared_ptr<Sprite> &sprite);

    /**
     * Constructs a sprite component from a pointer to a Sprite.
     * @param sprite The pointer to the sprite that is to be used. It will be converted to a shared pointer.
     */
    explicit SpriteComponent(Sprite *sprite);

    /**
     * Gets the underlying sprite
     * @return A shared pointer to the sprite
     */
    std::shared_ptr<Sprite> &getSprite();


    void setSprite(std::shared_ptr<Sprite> &&s);
    void setSprite(Sprite *s);
};
