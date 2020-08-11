#pragma once

#include "../State.h"
#include "../../util/ui/Button.h"

class GameOverState : public State {
    std::vector<Button> buttons;

public:
    GameOverState();

    void render();
    const std::vector<Button> &getButtons() const;

};