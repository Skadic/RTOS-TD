#pragma once

#include <optional>
#include <entity/entity.hpp>
#include "../components/tilecomponents/Tower.h"

std::optional<entt::entity> closestTarget(Tower &towerData, entt::registry &registry);