
#include "../State.h"
#include "../../util/ui/Button.h"
#include "../abstractmenustate/AbstractMenuState.h"

/**
 * This state shows a brief tutorial on how to play the game
 */
class TutorialState : public AbstractMenuState {

public:
    /**
     * Constructs a new TutorialState
     */
    TutorialState();

    /**
     * Renders everything except for the buttons
     */
    void render();
};
