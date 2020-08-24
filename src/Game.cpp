//
// Created by skadic on 15.06.20.
//

#include "Game.h"
#include "states/gamestate/GameState.h"
#include "util/Log.h"
#include "util/GlobalConsts.h"
#include "states/mainmenu/MainMenuState.h"

#include <iostream>
#include <entity/registry.hpp>

extern "C" {
#include <FreeRTOS.h>
#include <TUM_Event.h>
#include <TUM_Utils.h>
#include <TUM_Sound.h>
}

Game::Game() :
        screenLock{xSemaphoreCreateMutex()},
        drawSignal{xSemaphoreCreateBinary()},
        swapBufferSignal{xSemaphoreCreateBinary()},
        stateOperationSignal{xSemaphoreCreateBinary()},
        input{InputHandler(), xSemaphoreCreateMutex()},
        stateToPush{nullptr, xSemaphoreCreateMutex()},
        stateOperation{NONE, xSemaphoreCreateMutex()}
    {
    drawSignal.unlock();
}


void Game::start(char *binPath) {
    // Get the path of the folder which contains the binary running the program
    auto path = tumUtilGetBinFolderPath(binPath);

    // Init event handling and drawing
    tumEventInit();
    tumDrawInit(path);
    tumSoundInit(path);

    // Spawn the task that will update the screen each frame
    TaskHandle_t swap_buffer = nullptr;
    xTaskCreate(swapBufferTask, "swap_buffer", 0, nullptr, 2, &swap_buffer);

    // Spawn the task that will handle input
    TaskHandle_t input = nullptr;
    xTaskCreate(inputTask, "input", 0, nullptr, 2, &input);

    // Spawn the task that will handle enqueued pushing and popping operations in the state machine
    TaskHandle_t states = nullptr;
    xTaskCreate(stateMachineTask, "states", 0, nullptr, 2, &input);

    /*
     * Push the main menu state to the stack
     * Do not (!) use the state machine's methods (pushStack and popStack) directly when operating from a task
     * of the state that will be pushed over or popped.
     * Rather use the enqueue methods in Game. This does not work here because the task handling the state push and pop
     * operations is not running yet
     */
    Game::getStateMachine().pushStack(new MainMenuState());

    // Starts the task scheduler to start running the tasks
    vTaskStartScheduler();

    // Delete all tasks
    vTaskDelete(swap_buffer);
    vTaskDelete(input);
    vTaskDelete(states);
    tumEventExit();
}


Game& Game::get() {
    // The singleton game instance
    static Game instance;
    return instance;
}

Semaphore &Game::getScreenLock() {
    return screenLock;
}

Semaphore &Game::getDrawSignal() {
    return drawSignal;
}


LockGuard<entt::registry> &Game::getActiveStateRegistry() {
    return stateMachine.activeState().getRegistry();
}

StateMachine &Game::getStateMachine() {
    return stateMachine;
}

Semaphore &Game::getSwapBufferSignal() {
    return swapBufferSignal;
}

LockGuard<InputHandler> &Game::getInput() {
    return input;
}

void Game::enqueueStatePush(State *state) {

    auto stateOperationResource = *stateOperation.lock(portMAX_DELAY);
    auto stateToPushResource = *stateToPush.lock(portMAX_DELAY);

    // Set the operation to be executed to "Push"
    stateOperationResource.set(PUSH);
    // Set the state to be pushed to the given state
    stateToPushResource.set(state);

    // Signal the state machine task to handle a state operation
    stateOperationSignal.unlock();
}

void Game::enqueueStatePop() {
    auto stateOpLock = *stateOperation.lock(portMAX_DELAY);

    // Set the operation to be executed to "Pop"
    stateOpLock.set(POP);

    // Signal the state machine task to handle a state operation
    stateOperationSignal.unlock();
}

void Game::enqueueStatePop2X() {
    auto stateOpLock = *stateOperation.lock(portMAX_DELAY);

    // Set the operation to be executed to "Pop twice"
    stateOpLock.set(POP2X);

    // Signal the state machine task to handle a state operation
    stateOperationSignal.unlock();
}

LockGuard<State *> &Game::getStateToPush() {
    return stateToPush;
}

LockGuard<StateChange> &Game::getStateOperation() {
    return stateOperation;
}

Semaphore &Game::getStateOperationSignal() {
    return stateOperationSignal;
}


void stateMachineTask(void *ptr) {
    auto &game = Game::get();
    auto &stateMachine = game.getStateMachine();

    auto &stateToPushMutex = game.getStateToPush();
    auto &stateOperationMutex = game.getStateOperation();

    auto lastWake = xTaskGetTickCount();

    while (true) {
        // Check if a state operation should be handled by locking the semaphore representing the signal to handle an operation
        if(game.getStateOperationSignal().lock(0)) {
            // First see if some operation has been enqueued
            if(std::optional<Resource<StateChange>> stateOperation = stateOperationMutex.lock()) {
                switch (**stateOperation) {
                    case NONE:
                        break;
                    case POP: // If a Pop operation has been enqueued, pop a state off the stack
                        stateMachine.popStack();
                        (*game.getInput().lock(portMAX_DELAY))->resetPressedData();
                        break;
                    case PUSH: // If a Push operation has been enqueued, get the state to be pushed from the queue and push it
                        if(std::optional<Resource<State*>> stateToPush = stateToPushMutex.lock()) {
                            stateMachine.pushStack(**stateToPush);
                            (*stateToPush).set(nullptr);
                            (*game.getInput().lock(portMAX_DELAY))->resetPressedData();
                        }
                        break;
                    case POP2X: // This is used for when the current state wants to remove itself and the state below it from the stack
                        stateMachine.popStack2X();
                        (*game.getInput().lock(portMAX_DELAY))->resetPressedData();
                        break;
                    default: {}
                }
                (*stateOperation).set(NONE);
            }
        }

        // If there are no states left, the task scheduler should stop and the program should close
        if(stateMachine.empty()) vTaskEndScheduler();

        // Delay the task until the next frame
        vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
    }
}

void swapBufferTask(void *ptr) {
    Game& game = Game::get();

    auto lastWake = xTaskGetTickCount();

    // Get Draw access for this thread
    tumDrawBindThread();
    while (true) {
        if(game.getSwapBufferSignal().lock(portMAX_DELAY)) {
            // Take exclusive access to the screen
            if (game.getScreenLock().lock(portMAX_DELAY)) {
                // Update the Screen
                tumDrawUpdateScreen();

                // Release the screenLock and give a draw signal
                game.getScreenLock().unlock();
                game.getDrawSignal().unlock();
            }
        }
        // Delay until the next frame
        vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
    }
}

// The task that every frame receives the button input from the message queue
void inputTask(void *ptr) {
    auto &game = Game::get();
    auto &input = game.getInput();

    auto lastWake = xTaskGetTickCount();
    while (true) {
        // Update input
        if(auto inputOpt = input.lock()) {
            // Update events (like keyboard and mouse input)
            tumEventFetchEvents();
            (*inputOpt)->update();
            if ((*inputOpt)->keyDown(SDL_SCANCODE_ESCAPE)) {
                game.enqueueStatePop();
            }
        }
        // Delay until the next frame
        vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
    }
}
