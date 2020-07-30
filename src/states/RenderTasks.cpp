//
// Created by skadic on 30.07.20.
//

#include "RenderTasks.h"
#include "../util/GlobalConsts.h"
#include "../components/Health.h"
#include "../components/tilecomponents/Tower.h"
#include "../components/Enemy.h"
#include "../components/Hitbox.h"

void GameTasks::renderEntities(Renderer &renderer, entt::registry &registry) {
    auto renderView = registry.view<Position, SpriteComponent>();
    for(auto &entity : renderView) {
        Position &pos = renderView.get<Position>(entity);
        SpriteComponent &sprite = renderView.get<SpriteComponent>(entity);
        renderer.drawSprite(*sprite.getSprite(), pos.x, pos.y);
    }
}

void GameTasks::renderMap(Renderer &renderer, entt::registry &registry, Map &map) {
    // Get all tile entities from the registry
    auto tileView = registry.view<TilePosition, SpriteComponent>();
    // Render map
    for(auto &entity : tileView) {
        TilePosition &pos = tileView.get<TilePosition>(entity);
        SpriteComponent &sprite = tileView.get<SpriteComponent>(entity);
        renderer.drawSprite(*sprite.getSprite(), pos.x * TILE_SIZE, pos.y * TILE_SIZE);
    }

    // Render the map border
    renderer.drawBox(0, 0, map.getWidth() * TILE_SIZE, map.getHeight() * TILE_SIZE, 0x0000FF, false);
}

void GameTasks::renderTowerTargetConnections(Renderer &renderer, entt::registry &registry) {
    auto towerView = registry.view<TilePosition, Tower>();
    auto enemyView = registry.view<Position, Enemy>();

    for (auto &tower : towerView) {
        TilePosition &pos = towerView.get<TilePosition>(tower);
        Tower &towerData = towerView.get<Tower>(tower);

        std::set<entt::entity> &targets = towerData.getActualTargets();
        for (auto &target : targets) {
            if(registry.valid(target)) {
                Position &targetPos = enemyView.get<Position>(target);
                renderer.drawLine(pos.x * TILE_SIZE + TILE_SIZE / 2, pos.y * TILE_SIZE + TILE_SIZE / 2, targetPos.x + PLAYER_SIZE / 2, targetPos.y + PLAYER_SIZE / 2, 2, 0x800020);
            }
        }
    }
}

void GameTasks::renderHealth(Renderer &renderer, entt::registry &registry) {
    auto healthView = registry.view<Position, Health>();

    for (auto &entity : healthView) {
        Position &pos = healthView.get<Position>(entity);
        Health &health = healthView.get<Health>(entity);

        renderer.drawPie(pos.x, pos.y, 5, -90, -90 + 360 * (health.value / (double) health.maxHealth), 0x00FF00, true);
        renderer.drawPie(pos.x, pos.y, 4, -90 + 360 * (health.value / (double) health.maxHealth), -90, 0xFF0000, true);
    }
}

void GameTasks::renderRanges(Renderer &renderer, entt::registry &registry) {
    auto rangeView = registry.view<TilePosition, SpriteComponent, Range>();

    // Render Ranges
    for (auto &entity : rangeView) {
        TilePosition &pos = rangeView.get<TilePosition>(entity);
        SpriteComponent &sprite = rangeView.get<SpriteComponent>(entity);
        Range &range = rangeView.get<Range>(entity);

        renderer.drawCircle(pos.x * TILE_SIZE + TILE_SIZE / 2, pos.y * TILE_SIZE + TILE_SIZE / 2, range.radius, 0xFFFF00, false);
    }
}