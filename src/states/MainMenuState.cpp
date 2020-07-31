//
// Created by skadic on 31.07.20.
//

#include "MainMenuState.h"
#include "../util/GlobalConsts.h"
#include "MainMenuTasks.h"
#include <iostream>

void MainMenuState::initTasks() {
    addTask(MainMenuTasks::inputTask, "in", DEFAULT_TASK_STACK_SIZE, 0);
    addTask(MainMenuTasks::renderTask, "render", DEFAULT_TASK_STACK_SIZE, 0);
}



MainMenuState::MainMenuState() : State() {
    Button start = Button{"Start", 0, 0, 400, 100, [] () {std::cout << "Start" << std::endl;}};
    buttons.push_back(start);

    initTasks();
    suspendTasks();
}

const std::vector<Button> &MainMenuState::getButtons() const {
    return buttons;
}
