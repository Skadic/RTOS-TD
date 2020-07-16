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
     * Handles movement and collisions
     * @param statePointer Pointer to the GameState object executing the task
     */
    void gameMoveTask(void *statePointer);

    /**
     * Handles getting input related to controlling the player and moving the player accordingly
     * @param statePointer Pointer to the GameState object executing the task
     */
    void gameControlPlayerTask(void *statePointer);

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

    void gameWaveTask(void *statePointer);
}