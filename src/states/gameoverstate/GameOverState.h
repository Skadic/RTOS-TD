#pragma once

#include "../State.h"
#include "../../util/ui/Button.h"

/**
 * This state appears when the nexus loses all it's health and the game ends
 */
class GameOverState : public State {
    /**
     * The buttons of this menu
     */
    std::vector<Button> buttons;

public:
    /**
     * Constructs a new GameOverState
     */
    GameOverState();

    /**
     * Renders everything except for the buttons
     */
    void render();

    /**
     * Gets a vector of all the buttons of this state
     */
    const std::vector<Button> &getButtons() const;

};