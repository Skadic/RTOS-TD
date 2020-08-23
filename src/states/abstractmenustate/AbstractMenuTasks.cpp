//
// Created by skadic on 23.08.20.
//

#include "../../Game.h"
#include "../../util/GlobalConsts.h"
#include "AbstractMenuTasks.h"
#include "AbstractMenuState.h"

void AbstractMenuTasks::buttonClickTask(void *statePointer) {
    // Extract the AbstractMenuState from the void pointer
    AbstractMenuState &state = *static_cast<AbstractMenuState*>(statePointer);
    Game &game = Game::get();
    auto lastWake = xTaskGetTickCount();

    // Get the InputHandler from the game instance, guarded by a LockGuard
    auto &inputMutex = game.getInput();

    while(true) {
        // Lock the mutex. Because of how std::optional works, the "if" is only entered, if the lock actually returned a value
        if(auto inputOpt = inputMutex.lock()) {
            // Extract the InputHandler from the optional
            auto &input = *inputOpt;

            // Check whether the mouse position is on any of the buttons and left is clicked. In that case, call the button's
            // on-click function
            for(auto &button : state.getButtons()) {
                if(std::clamp(input->getMouseX(), button.x, (short) (button.x + button.w)) == input->getMouseX() &&
                   std::clamp(input->getMouseY(), button.y, (short) (button.y + button.h)) == input->getMouseY()
                   && input->leftClickDown()) {
                    button.onClick();
                }
            }
        }

        // Delay the task until the next frame
        vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
    }
}

void AbstractMenuTasks::stateRenderTask(void *statePointer) {
    // Extract the AbstractMenuState from the void pointer
    AbstractMenuState &state = *static_cast<AbstractMenuState*>(statePointer);
    Game &game = Game::get();
    auto lastWake = xTaskGetTickCount();

    while(true) {
        if(game.getDrawSignal().lock(portMAX_DELAY)) {
            if(game.getScreenLock().lock(portMAX_DELAY)){
                tumDrawClear(0x000000);

                // Render every button and the text on top of it
                for (const Button &b : state.getButtons()) {
                    state.getRenderer().drawBox(b.x, b.y, b.w, b.h, 0xFFFFFF, true);
                    state.getRenderer().drawText(const_cast<char*>(b.name.c_str()), b.x + b.w / 2 - b.name.length() * 3, b.y + b.h / 2 - 10, 0x000000);
                }

                // Render everything else
                state.render();

                // Free the screen to be used by other tasks, and give the signal to swap buffers
                game.getScreenLock().unlock();
                game.getSwapBufferSignal().unlock();
            }

        }

        // Delay the task until the next frame
        vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
    }
}
