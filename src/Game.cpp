//
// Created by skadic on 15.06.20.
//

#include "Game.h"
#include "states/TestState.h"
#include "states/GameState.h"
#include "util/GlobalConsts.h"
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <TUM_Event.h>
#include <TUM_Draw.h>
#include <TUM_Utils.h>
#include <SDL2/SDL_scancode.h>


#include <iostream>
#include <entity/registry.hpp>

Game::Game() :
    screenLock{xSemaphoreCreateMutex()},
    drawSignal{xSemaphoreCreateBinary()},
    drawHitboxSignal{xSemaphoreCreateBinary()}
    {
    // Create semaphores to synchronize access to the draw commands and for the swap_buffer task to give a draw signal
    drawSignal.unlock();
}

Game::~Game() = default;

void Game::start(char *binPath) {
    // Get the path of the folder which contains the binary running the program
    auto path = tumUtilGetBinFolderPath(binPath);

    // Init event handling and drawing
    tumEventInit();
    tumDrawInit(path);

    // Spawn the task that will update the screen each frame
    TaskHandle_t swap_buffer = nullptr;
    xTaskCreate(swapBufferTask, "swap_buffer", 0, nullptr, 2, &swap_buffer);

    TaskHandle_t input = nullptr;
    xTaskCreate(inputTask, "input", 0, nullptr, 2, &input);

    //GameState gameState(10, 10);
    Game::getStateMachine().pushStack(new GameState(10, 10));

    // Starts the task scheduler to start running the tasks
    vTaskStartScheduler();

    // Delete all tasks
    vTaskDelete(swap_buffer);
    //vTaskDelete(state_machine);
}


Game& Game::get() {
    static Game instance;
    return instance;
}

Semaphore &Game::getScreenLock() {
    return screenLock;
}

Semaphore &Game::getDrawSignal() {
    return drawSignal;
}

Semaphore &Game::getDrawHitboxSignal() {
    return drawHitboxSignal;
}

LockGuard<entt::registry> &Game::getActiveStateRegistry() {
    return stateMachine.activeState().getRegistry();
}

StateMachine &Game::getStateMachine() {
    return stateMachine;
}

void swapBufferTask(void *ptr) {

    Game& game = Game::get();

    auto lastWake = xTaskGetTickCount();

    // Get Draw access for this thread
    tumDrawBindThread();
    while (true) {
        //if(xSemaphoreTake(game.getSwapBufferSignal(), 0) == pdTRUE) {
            // Take exclusive access to the screen
            if (game.getScreenLock().lock(portMAX_DELAY)) {
                // Update the Screen
                tumDrawUpdateScreen();

                // Update events (like keyboard and mouse input)
                tumEventFetchEvents();

                // Release the screenLock and give a draw signal
                game.getScreenLock().unlock();
                game.getDrawSignal().unlock();
            }
        //}
        // Delay until the next frame
        vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
    }
}


void inputTask(void *ptr) {

    static unsigned char buttons[SDL_NUM_SCANCODES];
    static bool added = false;
    auto &stateMachine = Game::get().getStateMachine();
    auto lastWake = xTaskGetTickCount();
    while (true) {
        xQueueReceive(buttonInputQueue, &buttons, 0);

        if(!added && buttons[SDL_SCANCODE_W]) {
            stateMachine.pushStack(new TestState(false));
            added = true;
        }

        if(added && buttons[SDL_SCANCODE_S]) {
            stateMachine.popStack();
            added = false;
        }

        // Delay until the next frame
        vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
    }
}