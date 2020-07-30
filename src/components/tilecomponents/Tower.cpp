//
// Created by skadic on 10.07.20.
//

#include "Tower.h"

void Tower::setPotentialTargets(std::set<entt::entity> &newTargets) {
    potentialTargets.clear();
    for (entt::entity target : newTargets) {
        potentialTargets.insert(target);
    }
}

std::set<entt::entity> &Tower::getPotentialTargets() {
    return potentialTargets;
}

bool Tower::hasPotentialTargets() {
    return !potentialTargets.empty();
}

void Tower::setActualTargets(std::set<entt::entity> &newTargets) {
    actualTargets.clear();
    for (entt::entity target : newTargets) {
        actualTargets.insert(target);
    }
}

std::set<entt::entity> &Tower::getActualTargets() {
    return actualTargets;
}
