//
// Created by skadic on 19.06.20.
//

#include "AI.h"

AIComponent::AIComponent(AI *ai) : ai{std::unique_ptr<AI>(ai)} {}

std::unique_ptr<AI> &AIComponent::getAI() {
    return ai;
}
