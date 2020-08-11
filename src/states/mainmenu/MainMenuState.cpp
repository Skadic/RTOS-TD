//
// Created by skadic on 31.07.20.
//

#include "MainMenuState.h"
#include "../../util/GlobalConsts.h"
#include "../../Game.h"
#include "../gamestate/GameState.h"
#include "MainMenuTasks.h"
#include <iostream>

MainMenuState::MainMenuState() : State() {
    Button start = Button{"Start", SCREEN_WIDTH / 2 - MENU_BUTTON_WIDTH / 2, 200, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT,
                          [] () {
        Game::get().enqueueStatePush(new GameState("testmap.json"));
    }};

    Button exit = Button{"Exit", SCREEN_WIDTH / 2 - MENU_BUTTON_WIDTH / 2, 400, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT,
                          [] () {
        Game::get().enqueueStatePop();
    }};

    buttons.push_back(start);
    buttons.push_back(exit);

    tasks.push_back(nullptr); // Make space for another task handle
    xTaskCreate(MainMenuTasks::buttonRenderTask, "button_render", DEFAULT_TASK_STACK_SIZE, this, 0, &tasks.back());
    tasks.push_back(nullptr); // Make space for another task handle
    xTaskCreate(MainMenuTasks::buttonClickTask, "button_click", DEFAULT_TASK_STACK_SIZE, this, 0, &tasks.back());
    suspendTasks();
}

void MainMenuState::render() {}

const std::vector<Button> &MainMenuState::getButtons() const {
    return buttons;
}
