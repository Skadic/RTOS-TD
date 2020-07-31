//
// Created by skadic on 31.07.20.
//

#include "MainMenuTasks.h"
#include "MainMenuState.h"
#include "../Game.h"
#include "../util/GlobalConsts.h"


void MainMenuTasks::inputTask(void *statePointer) {
    MainMenuState &state = *static_cast<MainMenuState*>(statePointer);
    Game &game = Game::get();
    auto lastWake = xTaskGetTickCount();
    auto &inputMutex = game.getInput();

    while(true) {

        if(auto inputOpt = inputMutex.lock()) {
            auto &input = *inputOpt;
            for(auto &button : state.getButtons()) {
                if(std::clamp(input->getMouseX(), button.x, (short) (button.x + button.w)) == input->getMouseX() &&
                        std::clamp(input->getMouseY(), button.y, (short) (button.y + button.h)) == input->getMouseY()
                        && input->leftClickDown()) {
                    button.onClick();
                }
            }
        }

        vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
    }
}

void MainMenuTasks::renderTask(void *statePointer) {
    MainMenuState &state = *static_cast<MainMenuState*>(statePointer);
    Game &game = Game::get();
    auto lastWake = xTaskGetTickCount();

    while(true) {
        if(game.getDrawSignal().lock(portMAX_DELAY)) {
            if(game.getScreenLock().lock(portMAX_DELAY)){
                //std::cout << "RenderMenu" << std::endl;
                tumDrawClear(0x000000);

                for (const Button &b : state.getButtons()) {
                    state.getRenderer().drawBox(b.x, b.y, b.w, b.h, 0xFFFFFF, true);
                    state.getRenderer().drawText(const_cast<char*>(b.name.c_str()), b.x + b.w / 2 - b.name.length() * 3, b.y + b.h / 2 - 10, 0x000000);
                }

                game.getScreenLock().unlock();
                game.getSwapBufferSignal().unlock();
            }

        }
        vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
    }
}
