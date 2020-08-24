//
// Created by skadic on 19.06.20.
//

extern "C" {
#include <TUM_Event.h>
}
#include "InputHandler.h"
#include <iostream>

InputHandler::InputHandler() :
        inputCurrent{std::make_unique<std::vector<unsigned char>>(std::vector<unsigned char>{})},
        inputOld{std::make_unique<std::vector<unsigned char>>(std::vector<unsigned char>{})},
        mouseX{0},
        mouseY{0},
        leftClickCurrent{false},
        rightClickCurrent{false},
        middleClickCurrent{false},
        leftClickOld{false},
        rightClickOld{false},
        middleClickOld{false}
    {
    // Resize the input vectors to be able to hold the necessary amount of Scancodes
    inputCurrent->resize(SDL_NUM_SCANCODES);
    inputOld->resize(SDL_NUM_SCANCODES);
};

bool InputHandler::keyPressed(SDL_Scancode sdlScancode) {
    return inputCurrent->at(sdlScancode);
}

bool InputHandler::keyDown(SDL_Scancode sdlScancode) {
    // The key has just been pressed if the key is pressed right now, and was not pressed before
    return inputCurrent->at(sdlScancode) && !inputOld->at(sdlScancode);
}

bool InputHandler::keyUp(SDL_Scancode sdlScancode) {
    // The key has just been released if the key is not pressed right now, but was pressed before
    return !inputCurrent->at(sdlScancode) && inputOld->at(sdlScancode);
}

void InputHandler::update() {
    static bool changeNextTick = false;

    // See if there is been new input. If so, put the new data in the old vector and swap the contents of the vectors
    if (xQueueReceive(buttonInputQueue, this->inputOld->data(), 0)) {
        inputCurrent.swap(inputOld);

        // Swap the new and old input values
        std::swap(leftClickCurrent, leftClickOld);
        std::swap(rightClickCurrent, rightClickOld);
        std::swap(middleClickCurrent, middleClickOld);

        // Overwrite the new input values (which are currently occupied by the actual old values)
        leftClickCurrent = tumEventGetMouseLeft();
        rightClickCurrent = tumEventGetMouseRight();
        middleClickCurrent = tumEventGetMouseMiddle();

        changeNextTick = true;
    } else {
        if(changeNextTick) {
            resetPressedData();
            changeNextTick = false;
        }
    }

    // Retrieve the mouse X and Y position from the tum library
    mouseX = tumEventGetMouseX();
    mouseY = tumEventGetMouseY();
}

short InputHandler::getMouseX() {
    return mouseX;
}

short InputHandler::getMouseY() {
    return mouseY;
}

bool InputHandler::leftClicked() {
    return leftClickCurrent;
}

bool InputHandler::rightClicked() {
    return rightClickCurrent;
}

bool InputHandler::middleClicked() {
    return middleClickCurrent;
}

bool InputHandler::leftClickDown() {
    return leftClickCurrent && !leftClickOld;
}

bool InputHandler::rightClickDown() {
    return rightClickCurrent && !rightClickOld;
}

bool InputHandler::middleClickDown() {
    return middleClickCurrent && !middleClickOld;
}

bool InputHandler::leftClickUp() {
    return !leftClickCurrent && leftClickOld;
}

bool InputHandler::rightClickUp() {
    return !rightClickCurrent && rightClickOld;
}

bool InputHandler::middleClickUp() {
    return !middleClickCurrent && middleClickOld;
}

void InputHandler::resetPressedData() {
    leftClickOld = leftClickCurrent;
    middleClickOld = middleClickCurrent;
    rightClickOld = rightClickCurrent;

    std::copy(inputCurrent->begin(), inputCurrent->end(), inputOld->begin());
}
