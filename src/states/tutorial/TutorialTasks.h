#pragma once

namespace TutorialTasks {
    /**
     * Handles clicking on buttons
     * @param statePointer A pointer to the TutorialState
     */
    void buttonClickTask(void *statePointer);

    /**
     * Handles rendering the buttons
     * @param statePointer A pointer to the TutorialState
     */
    void buttonRenderTask(void *statePointer);
}