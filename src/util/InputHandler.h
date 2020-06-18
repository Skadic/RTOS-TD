#pragma once

#include <vector>
#include <SDL_scancode.h>

class InputHandler {
    std::vector<unsigned char> buttons;
    short mouseX;
    short mouseY;
    bool leftClick;
    bool rightClick;
    bool middleClick;


public:
    InputHandler();

    void update();

    bool buttonPressed(SDL_Scancode sdlScancode);

    short getMouseX();
    short getMouseY();
    bool leftClicked();
    bool rightClicked();
    bool middleClicked();

};
