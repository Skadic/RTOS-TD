//
// Created by skadic on 31.07.20.
//

#include "MainMenuState.h"
#include "../../util/GlobalConsts.h"
#include "../../Game.h"
#include "../gamestate/GameState.h"
#include "../tutorial/TutorialState.h"
#include <iostream>
extern "C" {
#include <FreeRTOS.h>
#include <TUM_Sound.h>
}

MainMenuState::MainMenuState() : AbstractMenuState() {
    // The buttons' y offset
    const auto yOffset = 200;
    // The margin between buttons
    const auto margin = 20;
    // The height of a button with the margin included
    const auto totalHeight = margin + MENU_BUTTON_HEIGHT;

    // A button that takes you to the game
    Button start = Button{"Start", SCREEN_WIDTH / 2 - MENU_BUTTON_WIDTH / 2, yOffset, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT,
                          [] () {
                              tumSoundPlaySample(click);
        Game::get().enqueueStatePush(new GameState("labyrinth.json"));
    }};

    // A button that takes you to the tutorial screen
    Button tutorial = Button{"Tutorial", SCREEN_WIDTH / 2 - MENU_BUTTON_WIDTH / 2, yOffset + totalHeight, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT,
                             [] () {
        tumSoundPlaySample(click);
        Game::get().enqueueStatePush(new TutorialState());
    }};

    // A button that closes the game
    Button exit = Button{"Exit", SCREEN_WIDTH / 2 - MENU_BUTTON_WIDTH / 2, yOffset + 2 * totalHeight, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT,
                         [] () {
        tumSoundPlaySample(click);
        // The main menu state is the only state on the stack. If the stack is empty, the game closes. See the state machine task in Game
        Game::get().enqueueStatePop();
    }};

    // Add the buttons to the vector
    buttons.push_back(start);
    buttons.push_back(tutorial);
    buttons.push_back(exit);
}

void MainMenuState::render() {
    renderer.drawText("ANTIBODY", SCREEN_WIDTH / 2 - 40, 100, 0xFFFFFF);
}
