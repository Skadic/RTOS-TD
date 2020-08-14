#pragma once

#include <functional>
#include <string>

/**
 * A simple button with position, size and an on click handler.
 * Rendering and detecting clicks needs to be done externally
 */
struct Button {
    /**
     * The button's name
     */
    std::string name;

    /**
     * The button's x position
     */
    short x;

    /**
     * The button's y position
     */
    short y;

    /**
     * The button's width
     */
    short w;

    /**
     * The button's height
     */
    short h;

    /**
     * The button's on click handler. This can be executed when a click is detected.
     * This is done externally by a task for example
     */
    const std::function <void ()> onClick;

public:
    /**
     * Creates a button with the specified data.
     */
    Button(std::string name, short x, short y, short w, short h, std::function<void()> onClick);
};

