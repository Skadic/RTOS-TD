#pragma once

namespace GameOverTasks {
    /**
     * Handles clicking on buttons
     * @param statePointer A pointer to the GameOverState
     */
    void buttonClickTask(void *statePointer);

    /**
     * Handles rendering the buttons
     * @param statePointer A pointer to the GameOverState
     */
    void buttonRenderTask(void *statePointer);
}