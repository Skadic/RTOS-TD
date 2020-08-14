#pragma once

namespace MainMenuTasks {
    /**
     * Handles clicking on buttons
     * @param statePointer A pointer to the MainMenuState
     */
    void buttonClickTask(void *statePointer);

    /**
     * Handles rendering the buttons
     * @param statePointer A pointer to the MainMenuState
     */
    void buttonRenderTask(void *statePointer);
}