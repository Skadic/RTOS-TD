#pragma once

#include "../State.h"
#include "../../util/ui/Button.h"
#include <set>


/**
 * A state used for menus. An instance of the state will automatically render the buttons added to its "buttons" vector
 * and handle their on-click events. For all other rendering please override the render method.
 *
 * However, nothing prohibits a user from adding additional tasks etc. to this type of state.
 */
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