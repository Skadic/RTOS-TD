#pragma once

#include "../../util/ui/Button.h"
#include "../abstractmenustate/AbstractMenuState.h"

/**
 * This state appears when the nexus loses all it's health and the game ends
 */
class GameOverState : public AbstractMenuState {

public:

    /**
     * Constructs a new GameOverState
     */
    GameOverState();

    /**
     * Renders everything except for the buttons
     */
    void render() override;
};