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
#include "GameOverTasks.h"

GameOverState::GameOverState() : State() {
    Button back = Button{"Back", SCREEN_WIDTH / 2 - MENU_BUTTON_WIDTH / 2, 400, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT,
                         [] () {
        tumSoundPlaySample(click);
        Game::get().enqueueStatePop2X();
    }};

    buttons.push_back(back);

    tasks.push_back(nullptr); // Make space for another task handle
    xTaskCreate(GameOverTasks::buttonRenderTask, "button_render", DEFAULT_TASK_STACK_SIZE, this, 0, &tasks.back());
    tasks.push_back(nullptr); // Make space for another task handle
    xTaskCreate(GameOverTasks::buttonClickTask, "button_click", DEFAULT_TASK_STACK_SIZE, this, 0, &tasks.back());
    suspendTasks();
}

void GameOverState::render() {
    renderer.drawText("Game Over", SCREEN_WIDTH / 2 - 35, 200, 0xFFFFFF);
}

const std::vector<Button> &GameOverState::getButtons() const {
    return buttons;
}

