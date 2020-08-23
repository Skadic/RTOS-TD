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
    auto offset = 200;
    auto margin = 20;
    auto totalHeight = margin + MENU_BUTTON_HEIGHT;

    Button start = Button{"Start", SCREEN_WIDTH / 2 - MENU_BUTTON_WIDTH / 2, offset, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT,
                          [] () {
                              tumSoundPlaySample(click);
        Game::get().enqueueStatePush(new GameState("testmap.json"));
    }};

    Button tutorial = Button{"Tutorial", SCREEN_WIDTH / 2 - MENU_BUTTON_WIDTH / 2, offset + totalHeight, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT,
                          [] () {
        tumSoundPlaySample(click);
        Game::get().enqueueStatePush(new TutorialState());
    }};

    Button exit = Button{"Exit", SCREEN_WIDTH / 2 - MENU_BUTTON_WIDTH / 2, offset + 2 * totalHeight, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT,
                          [] () {
        tumSoundPlaySample(click);
        Game::get().enqueueStatePop();
    }};

    buttons.push_back(start);
    buttons.push_back(tutorial);
    buttons.push_back(exit);
}

void MainMenuState::render() {
    renderer.drawText("ANTIBODY", SCREEN_WIDTH / 2 - 40, 100, 0xFFFFFF);
}
