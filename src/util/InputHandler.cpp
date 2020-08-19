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
    return inputCurrent->at(sdlScancode) && !inputOld->at(sdlScancode);
}

bool InputHandler::keyUp(SDL_Scancode sdlScancode) {
    return !inputCurrent->at(sdlScancode) && inputOld->at(sdlScancode);
}

void InputHandler::update() {
    static bool changeNextTick = false;

    inputCurrent.swap(inputOld);
    if (xQueueReceive(buttonInputQueue, this->inputCurrent->data(), 0)) {
        std::swap(leftClickCurrent, leftClickOld);
        std::swap(rightClickCurrent, rightClickOld);
        std::swap(middleClickCurrent, middleClickOld);

        leftClickCurrent = tumEventGetMouseLeft();
        rightClickCurrent = tumEventGetMouseRight();
        middleClickCurrent = tumEventGetMouseMiddle();

        changeNextTick = true;
    } else {
        inputCurrent.swap(inputOld);
        if(changeNextTick) {
            resetPressedData();
            changeNextTick = false;
        }
    }
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
