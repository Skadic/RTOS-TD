#pragma once

#include "../State.h"
#include "../../util/ui/Button.h"

/**
 * The main menu
 */
class MainMenuState : public State {
    /**
     * The buttons of this menu
     */
    std::vector<Button> buttons;

public:
    /**
     * Constructs a new MainMenuState
     */
    MainMenuState();

    /**
     * Renders everything except for the buttons.
     */
    void render();

    /**
     * Gets a vector of all the buttons of this state
     */
    const std::vector<Button> &getButtons() const;
};