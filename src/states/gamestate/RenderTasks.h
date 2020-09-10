#pragma once

#include <entity/registry.hpp>
#include "../../util/Renderer.h"
#include "../../util/map/Map.h"
#include "GameState.h"

namespace GameTasks {
    /**
     * Renders all Health displays for entities with Health
     * @param renderer The renderer of the state
     * @param registry The registry of the state
     */
    void renderHealth(Renderer &renderer, entt::registry &registry);

    /**
     * Renders the range of the tower hovered over with the mouse
     * @param renderer The renderer of the state
     * @param registry The registry of the state
     * @param map The map of the state
     */
    void renderHoveredRanges(Renderer &renderer, entt::registry &registry, Map &map);

    /**
     * Draws lines between towers and their targets
     * @param renderer The renderer of the state
     * @param registry The registry of the state
     */
    void renderTowerTargetConnections(Renderer &renderer, entt::registry &registry);

    /**
     * Renders all entities with a tile position and a sprite (which is basically the contents of the map)
     * @param state The game state that is executing the task
     * @param renderer The renderer of the state
     * @param registry The registry of the state
     * @param map The map to render
     */
    void renderMap(GameState &state, Renderer &renderer, entt::registry &registry, Map &map);

    /**
     * Renders all entities with a position and a sprite
     * @param renderer The renderer of the state
     * @param registry The registry of the state
     */
    void renderEntities(Renderer &renderer, entt::registry &registry);

    /**
     * Renders the path, the enemies will take
     * @param renderer The renderer of the state
     * @param path The path to render as a vector of TilePositions
     */
    void renderPath(Renderer &renderer, std::vector<TilePosition> &path);

    /**
     * Renders the HUD
     * @param state The game state that is executing the task
     * @param registry The registry of the state
     */
    void renderHUD(GameState &state, entt::registry &registry);
}