#pragma once

#include <vector>
#include <entity/entity.hpp>
#include <iostream>
#include <set>
#include "../AI/AI.h"

class entity;

class Tower {
    std::set<entt::entity> potentialTargets;
    std::set<entt::entity> actualTargets;

public:
    Tower() = default;

    void setActualTargets(std::set<entt::entity> &newTargets);
    std::set<entt::entity> &getActualTargets();
    void setPotentialTargets(std::set<entt::entity> &newTargets);
    std::set<entt::entity> &getPotentialTargets();
    bool hasPotentialTargets();
};