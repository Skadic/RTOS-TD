//
// Created by skadic on 10.07.20.
//

#include "Tower.h"

void Tower::setTargets(std::vector<entt::entity> &newTargets) {
    targets.clear();
    for (entt::entity &target : newTargets) {
        targets.insert(target);
    }
}

std::set<entt::entity> &Tower::getTargets() {
    return targets;
}

bool Tower::hasTargets() {
    return !targets.empty();
}
