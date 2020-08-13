//
// Created by skadic on 30.07.20.
//

#include "RenderTasks.h"
#include "../../util/GlobalConsts.h"
#include "../../components/Health.h"
#include "../../components/tilecomponents/Tower.h"
#include "../../components/Enemy.h"
#include "../../components/Hitbox.h"
#include "../../util/Wave.h"
#include "../../util/RenderUtils.h"
#include "GameState.h"
#include "../../Game.h"

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

void GameTasks::renderHoveredRanges(Renderer &renderer, entt::registry &registry, Map &map) {
    auto rangeView = registry.view<TilePosition, SpriteComponent, Range>();
    if(auto inputOpt = Game::get().getInput().lock()) {
        auto &input = *inputOpt;
        if(auto tileOpt = map.getMapTileAtScreenPos(input->getMouseX(), input->getMouseY(), renderer)) {

            if(registry.has<TilePosition, SpriteComponent, Range>(*tileOpt)) {
                TilePosition &pos = rangeView.get<TilePosition>(*tileOpt);
                SpriteComponent &sprite = rangeView.get<SpriteComponent>(*tileOpt);
                Range &range = rangeView.get<Range>(*tileOpt);

                renderer.drawCircle(pos.x * TILE_SIZE + TILE_SIZE / 2, pos.y * TILE_SIZE + TILE_SIZE / 2, range.radius, 0xFFFF00, false);
                renderer.drawBox(pos.x, pos.y, TILE_SIZE, TILE_SIZE, 0xFFFF00, false);
            }
        }

    }
}

void GameTasks::renderHUD(GameState &state, entt::registry &registry) {
    Map &map = state.getMap();
    Wave wave = state.getWave();
    TilePosition nexusPos = map.getNexusPosition();
    auto nexus = map.getMapTile(nexusPos.x, nexusPos.y);
    Health nexusHealth = registry.get<Health>(nexus);


    // Draw UI Boxes
    tumDrawFilledBox(0, 0, SCREEN_WIDTH, 50, UI_BG_COLOR);
    tumDrawBox(-1, -1, SCREEN_WIDTH + 2, 51, UI_BORDER_COLOR);
    tumDrawFilledBox(0, SCREEN_HEIGHT - 50, SCREEN_WIDTH, 51, UI_BG_COLOR);
    tumDrawBox(-1, SCREEN_HEIGHT - 50, SCREEN_WIDTH + 2, 51, UI_BORDER_COLOR);


    if (wave.isFinished()){
        tumDrawText("BUILDING PHASE", (SCREEN_WIDTH/2)-55, 25, 0xFFFFFF);
        drawInfo("Prepare yourself for Wave ", wave.getWaveNumber() + 1, (SCREEN_WIDTH/2)-85, 5);
    }else{
        drawInfo("Wave: ", wave.getWaveNumber(), (SCREEN_WIDTH/2)-25, 5);
        drawInfo("Enemies remaining: ", wave.getRemainingEnemies(),(SCREEN_WIDTH/2)-70, 25);
        drawInfo("Coins per enemy: ", wave.getEnemyCoins(), SCREEN_WIDTH-155, SCREEN_HEIGHT-25);
        drawInfo("Enemies health: ", ENEMY_BASE_HEALTH * wave.getEnemyHealthFactor() , SCREEN_WIDTH-155, SCREEN_HEIGHT-45);
    }

    drawInfo("Nexus Health: ", nexusHealth.value, SCREEN_WIDTH-125, 5);
    drawInfo("Coins: ", state.getCoins(), 5, 5);


    std::string text("Selected Block: ");
    text.append(getName(state.getTileTypeToPlace()));
    tumDrawText(strdup(text.c_str()), 5, SCREEN_HEIGHT-25, 0xFFFFFF);
}

void GameTasks::renderPath(Renderer &renderer, std::vector<TilePosition> &path) {
    if (path.empty()) return;
    auto color = [path](int i) {
        if(i < path.size() / 2) {
            return 0xFF0000 + ((0xFF * i / (path.size() / 2)) << 8);
        } else {
            i -= path.size() / 2;
            return 0x00FF00 + ((0xFF * (path.size() / 2 - i) / (path.size() / 2)) << 16);
        }
    };
    for (int i = 0; i < path.size() - 1; ++i) {
        TilePosition &tp1 = path[i];
        TilePosition &tp2 = path[i + 1];
        renderer.drawLine(
                tp1.x * TILE_SIZE + TILE_SIZE / 2,
                tp1.y * TILE_SIZE  + TILE_SIZE / 2,
                tp2.x * TILE_SIZE + TILE_SIZE / 2,
                tp2.y * TILE_SIZE + TILE_SIZE / 2,
                2,
                color(i));
    }
}
