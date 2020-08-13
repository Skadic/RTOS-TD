#pragma once

#include <entity/registry.hpp>
#include "../../util/Renderer.h"
#include "../../util/map/Map.h"
#include "GameState.h"

namespace GameTasks {
    void renderHealth(Renderer &renderer, entt::registry &registry);
    void renderRanges(Renderer &renderer, entt::registry &registry);
    void renderHoveredRanges(Renderer &renderer, entt::registry &registry, Map &map);
    void renderTowerTargetConnections(Renderer &renderer, entt::registry &registry);
    void renderMap(Renderer &renderer, entt::registry &registry, Map &map);
    void renderEntities(Renderer &renderer, entt::registry &registry);
    void renderPath(Renderer &renderer, std::vector<TilePosition> &path);
    void renderHUD(GameState &state, entt::registry &registry);
}