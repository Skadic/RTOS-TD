//
// Created by skadic on 23.08.20.
//

#include "../../Game.h"
#include "../../util/GlobalConsts.h"
#include "AbstractMenuTasks.h"
#include "AbstractMenuState.h"

void AbstractMenuTasks::buttonClickTask(void *statePointer) {
    AbstractMenuState &state = *static_cast<AbstractMenuState*>(statePointer);
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

void AbstractMenuTasks::stateRenderTask(void *statePointer) {
    AbstractMenuState &state = *static_cast<AbstractMenuState*>(statePointer);
    Game &game = Game::get();
    auto lastWake = xTaskGetTickCount();

    while(true) {
        if(game.getDrawSignal().lock(portMAX_DELAY)) {
            if(game.getScreenLock().lock(portMAX_DELAY)){
                //std::cout << "RenderMenu" << std::endl;
                tumDrawClear(0x000000);
                auto &buttons = state.getButtons();
                for (const Button &b : buttons) {
                    state.getRenderer().drawBox(b.x, b.y, b.w, b.h, 0xFFFFFF, true);
                    state.getRenderer().drawText(const_cast<char*>(b.name.c_str()), b.x + b.w / 2 - b.name.length() * 3, b.y + b.h / 2 - 10, 0x000000);
                }

                state.render();

                game.getScreenLock().unlock();
                game.getSwapBufferSignal().unlock();
            }

        }
        vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
    }
}
