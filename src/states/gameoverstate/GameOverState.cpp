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
    // A button that takes the player from the game over screen back to the main menu
    Button back = Button{"Back", SCREEN_WIDTH / 2 - MENU_BUTTON_WIDTH / 2, 400, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT,
                         [] () {
        tumSoundPlaySample(click);
        // When this button is clicked, the bottom state will be the main menu, then follows the game state, and the the game over state
        // So, popping the 2 topmost states off the stack will bring the user back to the main menu
        Game::get().enqueueStatePop2X();
    }};

    // Add the button to the state's buttons
    buttons.push_back(back);
}

void GameOverState::render() {
    renderer.drawText("Game Over", SCREEN_WIDTH / 2 - 35, 200, 0xFFFFFF);
}

