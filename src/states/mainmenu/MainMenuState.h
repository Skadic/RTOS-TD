#pragma once

#include "../../util/ui/Button.h"
#include "../abstractmenustate/AbstractMenuState.h"

/**
 * The main menu
 */
class MainMenuState : public AbstractMenuState {

public:
    /**
     * Constructs a new MainMenuState
     */
    MainMenuState();

    /**
     * Renders everything except for the buttons.
     */
    void render() override ;
};