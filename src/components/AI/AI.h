#pragma once

#include <memory>
#include <entity/registry.hpp>

class AI {
protected:
    entt::entity self;

public:
    virtual void act(entt::registry &registry) = 0;
};

class AIComponent {
    std::unique_ptr<AI> ai;

public:
    explicit AIComponent(AI *ai);
    std::unique_ptr<AI> &getAI();
};
