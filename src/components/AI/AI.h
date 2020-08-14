#pragma once

#include <memory>
#include <entity/registry.hpp>

/**
 * Represents the behavior of any computer-controlled entity
 * This can be enemies, towers, projectiles, etc.
 */
class AI {
protected:
    /**
     * The entity which this AI object is associated with
     */
    entt::entity self;

public:
    /**
     * Perform a single action. This can do basically anything, but as this is run every frame,
     * one should strive to make this somewhat efficient
     * @param registry The registry which the AI can retrieve data from
     */
    virtual void act(entt::registry &registry) = 0;
};

/**
 * A wrapper so AIs can be added as a component
 */
class AIComponent {
    std::unique_ptr<AI> ai;

public:
    explicit AIComponent(AI *ai);
    std::unique_ptr<AI> &getAI();
};
