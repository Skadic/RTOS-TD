#pragma once

#include "State.h"
#include "../util/ui/Button.h"

class MainMenuState : public State {

    std::vector<Button> buttons;
public:
    const std::vector<Button> &getButtons() const;

public:
    MainMenuState();

private:
    void initTasks();
};