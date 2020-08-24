#pragma once

#include "../components/Sprites.h"

/**
 * Handles rendering sprites and figures relative to an x and y offset and a scale
 * This is mostly used to realize the camera and render the game world.
 * This class transforms between game coordinates and screen coordinates to draw correctly.
 *
 * In this class we differentiate between screen coordinates and game coordinates.
 * The screen coordinates are the actual position on the screen to which will be drawn.
 * The game coordinates are the position in the game-world.
 */
class Renderer {
    /**
     * The x offset to render at
     */
    short xOffset;

    /**
     * The y offset to render at
     */
    short yOffset;

    /**
     * The scale to render at
     */
    float scale;

public:

    /**
     * Creates a Renderer with the specified parameters
     */
    explicit Renderer(short xOffset = 0, short yOffset = 0, float scale = 1);

    /**
     * Draws a line with the specified parameters
     * @param x1 The game x position of the start point of the line
     * @param y1 The game y position of the start point of the line
     * @param x2 The game x position of the end point of the line
     * @param y2 The game y position of the end point of the line
     * @param thickness The line's thickness
     * @param color The the line's color
     */
    void drawLine(short x1, short y1, short x2, short y2, unsigned char thickness, unsigned int color);

    /**
     * Draws a box with the specified parameters
     * @param x The box' game x position
     * @param y The box' game y position
     * @param width The box' width
     * @param height The box' height
     * @param color The box' color
     * @param filled Whether the box should be filled or not
     */
    void drawBox(short x, short y, short width, short height, unsigned int color, bool filled);

    /**
     * Draws a circle with the specified parameters
     * @param x The circle's game x position
     * @param y The circle's game y position
     * @param radius The circle's radius
     * @param color The circle's color
     * @param filled Whether the circle should be filled or not
     */
    void drawCircle(short x, short y, short radius, unsigned int color, bool filled);

    /**
     * Draws a pie with the specified parameters from the start angle to the end angle.
     * @param x The pie's game x position
     * @param y The pie's game y position
     * @param radius The pie's radius
     * @param start The beginning drawing angle in degrees (clockwise). 0 degrees points towards the right.
     * @param end The ending drawing angle in degrees (clockwise). 0 degrees points towards the right.
     * @param color The pie's color
     * @param filled Whether the pie should be filled or not
     */
    void drawPie(short x, short y, short radius, short start, short end, unsigned int color, bool filled);

    /**
     * Draws a Sprite object at the specified position
     * @param sprite The sprite to draw
     * @param x The game x position to draw to
     * @param y The game y position to draw to
     */
    void drawSprite(Sprite &sprite, short x, short y);

    /**
     * Draws text with the specified parameters
     * @param text The text to draw
     * @param x The game x position to draw to
     * @param y The game y position to draw to
     * @param color
     */
    void drawText(char* text, short x, short y, unsigned int color);


    void setScale(float s);
    void setOffset(short x, short y);
    void setXOffset(short x);
    void setYOffset(short y);

    void addOffset(short x, short y);
    void addXOffset(short x);
    void addYOffset(short y);

    short getXOffset();
    short getYOffset();
    float getScale();

    /**
     * Transforms an in-game x position to a screen x position
     * @param gameX The in-game x position
     * @return The position of the given game x position on the screen.
     */
    short getScreenX(short gameX);

    /**
     * Transforms an in-game y position to a screen y position
     * @param gameX The in-game y position
     * @return The position of the given game y position on the screen.
     */
    short getScreenY(short gameY);


    /**
     * Transforms an screen x position to a in-game x position
     * @param gameX The screen x position
     * @return The position of the given screen x position in the game world.
     */
    short getGameX(short screenX);

    /**
     * Transforms an screen y position to a in-game y position
     * @param gameX The screen y position
     * @return The position of the given screen y position in the game world.
     */
    short getGameY(short screenY);

private:

    /**
     * Checks if the described box (in in-game coordinates) is outside of the bounds of the screen and therefore not visible
     * @param x The in-game x coordinate of the box
     * @param y The in-game y coordinate of the box
     * @param width The width of the box
     * @param height The height of the box
     * @return true, if the box is outside of the bounds of the screen, false otherwise.
     */
    bool boxOutOfBounds(short x, short y, short width, short height);

    /**
     * Checks if the described circle (in in-game coordinates) is outside of the bounds of the screen and therefore not visible
     * @param x The in-game x coordinate of the circle
     * @param y The in-game x coordinate of the circle
     * @param radius The circle's radius
     * @return true, if the circle is outside of the bounds of the screen, false otherwise.
     */
    bool circleOutOfBounds(short x, short y, short radius);

    /**
     * Checks if the described line (in in-game coordinates) is outside of the bounds of the screen and therefore not visible
     * @param x1 The in-game x coordinate of the start point of the line
     * @param y1 The in-game y coordinate of the start point of the line
     * @param x2 The in-game x coordinate of the end point of the line
     * @param y2 The in-game y coordinate of the end point of the line
     * @return true, if the line is outside of the bounds of the screen, false otherwise.
     */
    bool lineOutOfBounds(short x1, short y1, short x2, short y2);
};
