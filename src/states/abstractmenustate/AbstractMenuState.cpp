//
// Created by skadic on 23.08.20.
//

#include "AbstractMenuState.h"
#include "../../util/GlobalConsts.h"
#include "AbstractMenuTasks.h"

const std::vector<Button> &AbstractMenuState::getButtons() const {
    return buttons;
}

AbstractMenuState::AbstractMenuState() : State(), buttons{std::vector<Button>()}{
    tasks.push_back(nullptr); // Make space for another task handle
    xTaskCreate(AbstractMenuTasks::stateRenderTask, "button_render", DEFAULT_TASK_STACK_SIZE, (AbstractMenuState*) this, 0, &tasks.back());
    vTaskSuspend(tasks.back());
    tasks.push_back(nullptr); // Make space for another task handle
    xTaskCreate(AbstractMenuTasks::buttonClickTask, "button_click", DEFAULT_TASK_STACK_SIZE, (AbstractMenuState*) this, 0, &tasks.back());
    vTaskSuspend(tasks.back());
}
