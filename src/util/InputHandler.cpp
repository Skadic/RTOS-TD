//
// Created by skadic on 19.06.20.
//

extern "C" {
#include <TUM_Event.h>
}
#include "InputHandler.h"

InputHandler::InputHandler() : mouseX{0}, mouseY{0}, leftClick{false}, rightClick{false}, middleClick{false} {
    buttons.resize(SDL_NUM_SCANCODES);
};

bool InputHandler::buttonPressed(SDL_Scancode sdlScancode) {
    return buttons[sdlScancode];
}

void InputHandler::update() {
    xQueueReceive(buttonInputQueue, this->buttons.data(), 0);
    mouseX = tumEventGetMouseX();
    mouseY = tumEventGetMouseY();
    leftClick = tumEventGetMouseLeft();
    rightClick = tumEventGetMouseRight();
    middleClick = tumEventGetMouseMiddle();
}

short InputHandler::getMouseX() {
    return mouseX;
}

short InputHandler::getMouseY() {
    return mouseY;
}

bool InputHandler::leftClicked() {
    return leftClick;
}

bool InputHandler::rightClicked() {
    return rightClick;
}

bool InputHandler::middleClicked() {
    return middleClick;
}
