//
// Created by skadic on 13.08.20.
//

#include <fstream>
#include "TutorialState.h"
#include "../../util/GlobalConsts.h"
#include "../../Game.h"
extern "C" {
#include <FreeRTOS.h>
#include <TUM_Sound.h>
}

TutorialState::TutorialState() : AbstractMenuState() {

    // A button that takes you back to the main menu
    Button back = Button{"Back", SCREEN_WIDTH / 2 - MENU_BUTTON_WIDTH / 2, SCREEN_HEIGHT - 2 * MENU_BUTTON_HEIGHT, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT,
                         [] () {
        tumSoundPlaySample(click);
        Game::get().enqueueStatePop();
    }};

    // Add the button to the menu
    buttons.push_back(back);
}

void TutorialState::render() {
    static std::vector<std::string> tutorialText;

    // if the tutorial text has not been loaded yet, load it from the tutorial.txt file
    if(tutorialText.empty()) {
        // Open an input stream
        std::ifstream tutorialIn("../resources/misc/tutorial.txt");
        if(tutorialIn.is_open()) {
            // Read each line and append it to the tutorialText string
            std::string line;
            while(getline(tutorialIn, line)) {
                tutorialText.push_back(line);
            }
        } else {
            std::cout << "Unable to open" << std::endl;
        }
        tutorialIn.close();
    }

    // Draw each line of the tutorial text
    for(int i = 0; i < tutorialText.size(); i++) {
        auto &line = tutorialText[i];
        renderer.drawText(const_cast<char*>(line.c_str()), SCREEN_WIDTH / 2 - 300, 150 + i * 25, 0xFFFFFF);
    }

}
