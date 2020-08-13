//
// Created by skadic on 31.07.20.
//

#include "MainMenuState.h"
#include "../../util/GlobalConsts.h"
#include "../../Game.h"
#include "../gamestate/GameState.h"
#include "MainMenuTasks.h"
#include "../tutorial/TutorialState.h"
#include <iostream>

MainMenuState::MainMenuState() : State() {
    auto offset = 200;
    auto margin = 20;
    auto totalHeight = margin + MENU_BUTTON_HEIGHT;

    Button start = Button{"Start", SCREEN_WIDTH / 2 - MENU_BUTTON_WIDTH / 2, offset, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT,
                          [] () {
        Game::get().enqueueStatePush(new GameState("testmap.json"));
    }};

    Button tutorial = Button{"Tutorial", SCREEN_WIDTH / 2 - MENU_BUTTON_WIDTH / 2, offset + totalHeight, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT,
                          [] () {
                              Game::get().enqueueStatePush(new TutorialState());
                          }};

    Button exit = Button{"Exit", SCREEN_WIDTH / 2 - MENU_BUTTON_WIDTH / 2, offset + 2 * totalHeight, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT,
                          [] () {
        Game::get().enqueueStatePop();
    }};

    buttons.push_back(start);
    buttons.push_back(tutorial);
    buttons.push_back(exit);

    tasks.push_back(nullptr); // Make space for another task handle
    xTaskCreate(MainMenuTasks::buttonRenderTask, "button_render", DEFAULT_TASK_STACK_SIZE, this, 0, &tasks.back());
    tasks.push_back(nullptr); // Make space for another task handle
    xTaskCreate(MainMenuTasks::buttonClickTask, "button_click", DEFAULT_TASK_STACK_SIZE, this, 0, &tasks.back());
    suspendTasks();
}

void MainMenuState::render() {
    renderer.drawText("CORONA DEFENSE", SCREEN_WIDTH / 2 - 60, 100, 0xFFFFFF);
}

const std::vector<Button> &MainMenuState::getButtons() const {
    return buttons;
}
