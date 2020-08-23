//
// Created by skadic on 23.08.20.
//

#pragma once

namespace AbstractMenuTasks {
    /**
     * Handles clicking on buttons
     * @param statePointer A pointer to the AbstractMenuState
     */
    void buttonClickTask(void *statePointer);

    /**
     * Handles rendering the buttons
     * @param statePointer A pointer to the AbstractMenuState
     */
    void stateRenderTask(void *statePointer);
}
