#pragma once

#include <vector>
#include <entity/entity.hpp>
#include <iostream>
#include <set>
#include "../AI/AI.h"

class entity;

class Tower {
    std::set<entt::entity> targets;

public:
    Tower() = default;

    void setTargets(std::vector<entt::entity> &newTargets);
    std::set<entt::entity> &getTargets();
    bool hasTargets();
};