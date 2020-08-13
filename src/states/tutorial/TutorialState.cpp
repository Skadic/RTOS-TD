//
// Created by skadic on 13.08.20.
//

#include <fstream>
#include "TutorialState.h"
#include "../../util/GlobalConsts.h"
#include "../../Game.h"
#include "TutorialTasks.h"

TutorialState::TutorialState() : State() {
    Button back = Button{"Back", SCREEN_WIDTH / 2 - MENU_BUTTON_WIDTH / 2, SCREEN_HEIGHT - 2 * MENU_BUTTON_HEIGHT, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT,
                         [] () {
                             Game::get().enqueueStatePop();
                         }};

    buttons.push_back(back);

    tasks.push_back(nullptr); // Make space for another task handle
    xTaskCreate(TutorialTasks::buttonRenderTask, "button_render", DEFAULT_TASK_STACK_SIZE, this, 0, &tasks.back());
    tasks.push_back(nullptr); // Make space for another task handle
    xTaskCreate(TutorialTasks::buttonClickTask, "button_click", DEFAULT_TASK_STACK_SIZE, this, 0, &tasks.back());
    suspendTasks();
}

void TutorialState::render() {
    static std::vector<std::string> s;

    if(s.empty()) {
        std::ifstream tutorialIn("../resources/misc/tutorial.txt");
        if(tutorialIn.is_open()) {
            std::string line;
            while(getline(tutorialIn, line)) {
                s.push_back(line);
            }
        } else {
            std::cout << "Unable to open" << std::endl;
        }
        tutorialIn.close();
    }

    for(int i = 0; i < s.size(); i++) {
        auto &line = s[i];
        renderer.drawText(const_cast<char*>(line.c_str()), SCREEN_WIDTH / 2 - 300, 150 + i * 25, 0xFFFFFF);
    }


}

const std::vector<Button> &TutorialState::getButtons() const {
    return buttons;
}
