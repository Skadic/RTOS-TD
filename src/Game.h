#pragma once

#include <entity/registry.hpp>
#include "states/StateMachine.h"
#include "util/sync/LockGuard.h"
#include "util/InputHandler.h"

extern "C" {
#include <FreeRTOS.h>
#include <semphr.h>
}

/**
 * The central game class
 */
class Game {

private:
    /**
     * A mutex that allows only 1 thread to access the screen at a time
     */
    Semaphore screenLock;

    /**
     * A binary semaphore which the swap buffer task uses to signal the currently active render task to start rendering
     */
    Semaphore drawSignal;

    /**
     * A binary semaphore which the currently active render task uses to signal the swap buffer task to swap buffers
     */
    Semaphore swapBufferSignal;

    /**
     * A binary semaphore which is used to signal the state machine task that an operation has to be handled
     */
    Semaphore stateOperationSignal;

    /**
     * The input handler guarded with a LockGuard
     */
    LockGuard<InputHandler> input;

    /**
     * The state machine which manages the state stack and currently active state
     */
    StateMachine stateMachine;

    /**
     * This is used to hold the state to be pushed to the state machine in case a push operation has been enqueued
     */
    LockGuard<State*> stateToPush;

    /**
     * This is used to hold the state operation to be executed when a stack operation has been enqueued
     */
    LockGuard<StateChange> stateOperation;

    /**
     * Constructs the game
     */
    Game();

public:

    /**
     * Gets the global game instance
     */
    static Game& get();

    /**
     * Starts the game
     * @param binPath The path of the game's binary
     */
    void start(char* binPath);

    Semaphore &getScreenLock();
    Semaphore &getDrawSignal();
    Semaphore &getSwapBufferSignal();
    Semaphore &getStateOperationSignal();

    StateMachine &getStateMachine();
    LockGuard<State*> &getStateToPush();
    LockGuard<StateChange> &getStateOperation();

    /**
     * Signals the state machine task to push a state to the state stack
     * @param state The state to push
     */
    void enqueueStatePush(State* state);

    /**
     * Signals the state machine task to pop a state off the state stack
     */
    void enqueueStatePop();

    /**
     * Signals the state machine task to pop 2 states off the state stack
     */
    void enqueueStatePop2X();

    /**
     * Get the active state's registry
     * @return The registry guarded by a lock guard
     */
    LockGuard<entt::registry> &getActiveStateRegistry();

    /**
     * Get the input handler
     * @return The input handler guarded by a lock guard
     */
    LockGuard<InputHandler> &getInput();

    /**
     * The game cannot be copy constructed
     */
    Game(Game const&)               = delete;

    /**
     * The game cannot be copied by assignment
     */
    void operator=(Game const&)     = delete;
};

/**
 * The task responsible for swapping screen buffers each frame
 * @param ptr Unused. All tasks need to have 1 void* argument
 */
void swapBufferTask(void *ptr);

/**
 * The task that handles the state machine.
 * Once there are state operations enqueued using the appropriate methods, this task will execute said operation.
 * @param ptr Unused. All tasks need to have 1 void* argument
 */
void stateMachineTask(void *ptr);

/**
 * Handles input and updates the input handler accordingly
 * @param ptr Unused. All tasks need to have 1 void* argument
 */
void inputTask(void *ptr);