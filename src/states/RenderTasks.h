#pragma once

#include <entity/registry.hpp>
#include "../util/Renderer.h"
#include "../util/map/Map.h"

namespace GameTasks {
    void renderHealth(Renderer &renderer, entt::registry &registry);
    void renderRanges(Renderer &renderer, entt::registry &registry);
    void renderTowerTargetConnections(Renderer &renderer, entt::registry &registry);
    void renderMap(Renderer &renderer, entt::registry &registry, Map &map);
    void renderEntities(Renderer &renderer, entt::registry &registry);
}