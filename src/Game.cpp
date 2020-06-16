//
// Created by skadic on 15.06.20.
//




#include "Game.h"
#include "states/TestState.h"

extern "C" {
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <TUM_Event.h>
#include <TUM_Draw.h>
#include <TUM_Utils.h>
}

#include <iostream>
#include <entity/registry.hpp>

Game::Game() : registry{Mutex(entt::registry(), xSemaphoreCreateMutex())} {
    // Create semaphores to synchronize access to the draw commands and for the swap_buffer task to give a draw signal
    screenLock = xSemaphoreCreateMutex();
    swapBufferSignal = xSemaphoreCreateBinary();
    drawSignal = xSemaphoreCreateBinary();
    drawHitboxSignal = xSemaphoreCreateBinary();
    xSemaphoreGive(swapBufferSignal);
}

Game::~Game() {
    vSemaphoreDelete(screenLock);
    vSemaphoreDelete(drawSignal);
    vSemaphoreDelete(drawHitboxSignal);
}

void Game::start(char *binPath) {

    // Get the path of the folder which contains the binary running the program
    auto path = tumUtilGetBinFolderPath(binPath);

    // Init event handling and drawing
    tumEventInit();
    tumDrawInit(path);

    // Spawn the task that will update the screen each frame
    TaskHandle_t swap_buffer = nullptr;
    xTaskCreate(swapBufferTask, "swap_buffer", 0, nullptr, 2, &swap_buffer);

    // Spawn the task that will be responsible for switching states
    TaskHandle_t state_machine = nullptr;
    xTaskCreate(stateMachineTask, "state_machine", 0, nullptr, 2, &state_machine);
    TestState testState;

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

SemaphoreHandle_t Game::getScreenLock() {
    return screenLock;
}

SemaphoreHandle_t Game::getDrawSignal() {
    return drawSignal;
}

SemaphoreHandle_t Game::getDrawHitboxSignal() {
    return drawHitboxSignal;
}

SemaphoreHandle_t Game::getSwapBufferSignal() {
    return swapBufferSignal;
}

Mutex<entt::registry> &Game::getRegistry() {
    return registry;
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
        if(xSemaphoreTake(game.getSwapBufferSignal(), 0) == pdTRUE) {
            std::cout << "swap buffer" << std::endl;
            // Take exclusive access to the screen
            if (xSemaphoreTake(game.getScreenLock(), portMAX_DELAY) == pdTRUE) {
                // Update the Screen
                tumDrawUpdateScreen();

                // Update events (like keyboard and mouse input)
                tumEventFetchEvents();

                // Release the screenLock and give a draw signal
                xSemaphoreGive(game.getScreenLock());
                xSemaphoreGive(game.getDrawSignal());
            }
        }
        // Delay until the next frame
        vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
    }
}

void stateMachineTask(void *stateMachinePointer) {
    StateMachine& stateMachine = Game::get().getStateMachine();

    auto lastWake = xTaskGetTickCount();
    while (true) {
        stateMachine.handleOperation();
        // Delay until the next frame
        vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
    }
}