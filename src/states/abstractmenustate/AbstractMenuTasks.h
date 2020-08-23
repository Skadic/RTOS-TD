//
// Created by skadic on 23.08.20.
//

#pragma once

/**
 * Tasks for basic interacting with buttons
 */
namespace AbstractMenuTasks {
    /**
     * Handles clicking on buttons
     * @param statePointer A pointer to the AbstractMenuState
     */
    void buttonClickTask(void *statePointer);

    /**
     * Handles rendering the state. The buttons will be automatically rendered. Everything else should be included in the
     * render method of AbstractMenuState
     * @param statePointer A pointer to the AbstractMenuState
     */
    void stateRenderTask(void *statePointer);
}
