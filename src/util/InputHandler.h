#pragma once

#include <vector>
#include <memory>
#include <SDL_scancode.h>

class InputHandler {
    std::unique_ptr<std::vector<unsigned char>> inputCurrent;
    short mouseX;
    short mouseY;
    bool leftClickCurrent;
    bool rightClickCurrent;
    bool middleClickCurrent;

    std::unique_ptr<std::vector<unsigned char>> inputOld;
    bool leftClickOld;
    bool rightClickOld;
    bool middleClickOld;

public:
    InputHandler();

    void update();



    short getMouseX();
    short getMouseY();

    bool leftClickDown();
    bool rightClickDown();
    bool middleClickDown();
    bool buttonDown(SDL_Scancode sdlScancode);

    bool leftClickUp();
    bool rightClickUp();
    bool middleClickUp();
    bool buttonUp(SDL_Scancode sdlScancode);

    bool leftClicked();
    bool rightClicked();
    bool middleClicked();
    bool buttonPressed(SDL_Scancode sdlScancode);
};
