//
// Created by skadic on 31.07.20.
//

#include "MainMenuState.h"
#include "../util/GlobalConsts.h"
#include "MainMenuTasks.h"
#include "../Game.h"
#include "GameState.h"
#include <iostream>

void MainMenuState::initTasks() {
    addTask(MainMenuTasks::inputTask, "in", DEFAULT_TASK_STACK_SIZE, 0);
    addTask(MainMenuTasks::renderTask, "render", DEFAULT_TASK_STACK_SIZE, 0);
}



MainMenuState::MainMenuState() : State() {
    static int buttonWidth = 400;
    static int buttonHeight = 100;
    Button start = Button{"Start", SCREEN_WIDTH / 2 - buttonWidth / 2, 200, buttonWidth, buttonHeight,
                          [] () {
        Game::get().enqueueStatePush(new GameState("testmap.json"));
    }};

    Button exit = Button{"Exit", SCREEN_WIDTH / 2 - buttonWidth / 2, 400, buttonWidth, buttonHeight,
                          [] () {
        Game::get().enqueueStatePop();
    }};

    buttons.push_back(start);
    buttons.push_back(exit);

    initTasks();
    suspendTasks();
}

const std::vector<Button> &MainMenuState::getButtons() const {
    return buttons;
}
