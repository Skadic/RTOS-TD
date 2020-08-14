
#include "../State.h"
#include "../../util/ui/Button.h"

/**
 * This state shows a brief tutorial on how to play the game
 */
class TutorialState : public State {
    /**
     * The buttons of this menu
     */
    std::vector<Button> buttons;

public:
    /**
     * Constructs a new TutorialState
     */
    TutorialState();

    /**
     * Renders everything except for the buttons
     */
    void render();

    /**
     * Gets a vector of all the buttons of this state
     */
    const std::vector<Button> &getButtons() const;
};
