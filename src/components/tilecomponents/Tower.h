#pragma once

#include <vector>
#include <entity/entity.hpp>
#include <iostream>
#include <set>
#include "../AI/AI.h"

class entity;

/**
 * A component representing a tower and relevant data
 */
class Tower {
    /**
     * All the targets that are within range of this tower. This is to be determined by a CollisionTable
     */
    std::set<entt::entity> potentialTargets;

    /**
     * The actual targets the Tower is firing at.
     * This is used to render the connections between tower and targets, and is usually populated in the AI of the tower.
     */
    std::set<entt::entity> actualTargets;

public:
    Tower() = default;

    void setActualTargets(std::set<entt::entity> &newTargets);
    std::set<entt::entity> &getActualTargets();
    void setPotentialTargets(std::set<entt::entity> &newTargets);
    std::set<entt::entity> &getPotentialTargets();
    bool hasPotentialTargets();
};