#pragma once

#include <vector>
#include <memory>
#include <SDL_scancode.h>

/**
 * Handles input from the TUM library
 */
class InputHandler {

    /**
     * The mouse X position
     */
    short mouseX;

    /**
     * The mouse Y position
     */
    short mouseY;

    /**
     * The current keyboard input
     */
    std::unique_ptr<std::vector<unsigned char>> inputCurrent;

    /**
     * Whether the left mouse button is pressed right now
     */
    bool leftClickCurrent;

    /**
     * Whether the right mouse button is pressed right now
     */
    bool rightClickCurrent;

    /**
     * Whether the middle mouse button is pressed right now
     */
    bool middleClickCurrent;

    // Equivalent old data. This is used for detecting whether buttons have been released or pressed down.

    /**
     * The old keyboard input
     */
    std::unique_ptr<std::vector<unsigned char>> inputOld;

    /**
     * Whether the left mouse button was pressed before
     */
    bool leftClickOld;

    /**
     * Whether the right mouse button was pressed before
     */
    bool rightClickOld;

    /**
     * Whether the middle mouse button was pressed before
     */
    bool middleClickOld;

public:
    /**
     * Creates a new InputHandler
     */
    InputHandler();

    /**
     * Retrieves new input data from the TUM library and updates all data
     */
    void update();

    /**
     * Sets the old data equal to the current data
     * This is to e.g. prevent for mouse clicks to persist across state changes.
     */
    void resetPressedData();

    /**
     * Gets the x position of the mouse on the screen
     */
    short getMouseX();

    /**
     * Gets the y position of the mouse on the screen
     */
    short getMouseY();

    /**
     * Returns whether the left mouse button has been pressed just now
     */
    bool leftClickDown();

    /**
     * Returns whether the right mouse button has been pressed just now
     */
    bool rightClickDown();

    /**
     * Returns whether the middle mouse button has been pressed just now
     */
    bool middleClickDown();

    /**
     * Returns whether a certain key has been pressed down just now
     * @param sdlScancode The SDL scancode of the key to check. These can be found in `SDL_scancode.h`
     * @return true, if the given key has been pressed down just now
     */
    bool keyDown(SDL_Scancode sdlScancode);

    /**
     * Returns whether the left mouse button has been released just now
     */
    bool leftClickUp();

    /**
     * Returns whether the right mouse button has been released just now
     */
    bool rightClickUp();

    /**
     * Returns whether the middle mouse button has been released just now
     */
    bool middleClickUp();

    /**
     * Returns whether a certain key has been released just now
     * @param sdlScancode The SDL scancode of the key to check. These can be found in `SDL_scancode.h`
     * @return true, if the given key has been released just now
     */
    bool keyUp(SDL_Scancode sdlScancode);


    /**
     * Returns whether the left mouse button is held down
     */
    bool leftClicked();

    /**
     * Returns whether the right mouse button is held down
     */
    bool rightClicked();

    /**
     * Returns whether the middle mouse button is held down
     */
    bool middleClicked();

    /**
     * Returns whether a certain key is pressed
     * @param sdlScancode The SDL scancode of the key to check. These can be found in `SDL_scancode.h`
     * @return true, if the given key is pressed
     */
    bool keyPressed(SDL_Scancode sdlScancode);
};
