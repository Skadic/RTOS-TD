#pragma once

#include "../State.h"
#include "../../util/ui/Button.h"
#include <set>

class AbstractMenuState : public State {

protected:

    /**
     * The buttons of this menu
     */
    std::vector<Button> buttons;

    /**
     * Constructs a new AbstractMenuState
     */
    AbstractMenuState();

public:

    /**
     * Renders everything except the buttons
     */
    virtual void render() {};

    /**
     * Gets a vector of all the buttons of this state
     */
    const std::vector<Button> &getButtons() const;
};