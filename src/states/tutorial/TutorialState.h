
#include "../State.h"
#include "../../util/ui/Button.h"

class TutorialState : public State {
    std::vector<Button> buttons;

public:
    TutorialState();

    void render();
    const std::vector<Button> &getButtons() const;
};
