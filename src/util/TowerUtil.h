#pragma once

#include <optional>
#include <entity/entity.hpp>
#include "../components/tilecomponents/Tower.h"

/**
 * Calculates the target closest to the goal that's still in range of the tower.
 *
 * @param towerData The tower data, that contains all the potential targets (i.e. all targets in range of the tower)
 * @param registry The registry to pull data from
 * @return std::nullopt if there are no potential targets. Otherwise returns the entity in range
 * that is closest to the goal wrapped in an optional
 */
std::optional<entt::entity> closestTarget(Tower &towerData, entt::registry &registry);