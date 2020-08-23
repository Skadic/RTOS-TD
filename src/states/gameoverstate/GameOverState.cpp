//
// Created by skadic on 11.08.20.
//

extern "C" {
#include <FreeRTOS.h>
#include <TUM_Sound.h>
}
#include "GameOverState.h"
#include "../../util/GlobalConsts.h"
#include "../../Game.h"

GameOverState::GameOverState() : AbstractMenuState() {
    Button back = Button{"Back", SCREEN_WIDTH / 2 - MENU_BUTTON_WIDTH / 2, 400, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT,
                         [] () {
        tumSoundPlaySample(click);
        Game::get().enqueueStatePop2X();
    }};

    buttons.push_back(back);
}

void GameOverState::render() {
    renderer.drawText("Game Over", SCREEN_WIDTH / 2 - 35, 200, 0xFFFFFF);
}

