#pragma once

/**
 * Contains all the Tasks required for the GameState
 */
namespace GameTasks {
    /**
     * Renders all the entities that have a Position and a SpriteComponent
     * @param statePointer Pointer to the GameState object executing the task
     */
    void gameRenderTask(void *statePointer);

    /**
     * Handles movement
     * @param statePointer Pointer to the GameState object executing the task
     */
    void gameMoveTask(void *statePointer);

    /**
     * Handles collisions
     * @param statePointer Pointer to the GameState object executing the task
     */
    void gameCollisionTask(void *statePointer);

    /**
     * Handles getting input from the keyboard
     * @param statePointer Pointer to the GameState object executing the task
     */
    void gameKeyboardInputTask(void *statePointer);

    /**
     * Handles anything related to mouse input. At this point, it's just placing towers though
     * @param statePointer Pointer to the GameState object executing the task
     */
    void gameMouseInputTask(void *statePointer);

    /**
     * Handles the execution of AI each tick
     * @param statePointer Pointer to the GameState object executing the task
     */
    void gameAITask(void *statePointer);

    /**
     * Spawns enemies
     * @param statePointer Pointer to the GameState object executing the task
     */
    void gameSpawnTask(void *statePointer);

    /**
     * Handles adding enemies in range of a tower to their Target List
     * @param statePointer Pointer to the GameState object executing the task
     */
    void gameTowerTask(void *statePointer);

    /**
     * Kills of entities with 0 health
     * @param statePointer Pointer to the GameState object executing the task
     */
    void gameKillTask(void *statePointer);

    /**
     * Handles whether a wave is finished and when to start a new wave
     * @param statePointer Pointer to the GameState object executing the task
     * @relates Wave
     */
    void gameWaveTask(void *statePointer);

    /**
     * Handles deleting entities which are flagged for deletion with the Delete flag
     * @param statePointer Pointer to the GameState object executing the task
     * @relates Delete
     */
    void gameDeleteTask(void *statePointer);
}