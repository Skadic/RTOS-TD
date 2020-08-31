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
#include "../../components/Upgrade.h"

void GameTasks::renderEntities(Renderer &renderer, entt::registry &registry) {
    // Get all entities with a Position and a Sprite
    auto renderView = registry.view<Position, SpriteComponent>();
    // Draw each of them
    for(auto &entity : renderView) {
        Position &pos = renderView.get<Position>(entity);
        SpriteComponent &sprite = renderView.get<SpriteComponent>(entity);
        renderer.drawSprite(*sprite.getSprite(), pos.x, pos.y);
    }
}

void GameTasks::renderMap(Renderer &renderer, entt::registry &registry, Map &map) {
    // Get all entities with a tile position and a sprite (= all tiles basically)
    auto tileView = registry.view<TilePosition, SpriteComponent>();
    // Draw each of them
    for(auto &entity : tileView) {
        TilePosition &pos = tileView.get<TilePosition>(entity);
        SpriteComponent &sprite = tileView.get<SpriteComponent>(entity);
        renderer.drawSprite(*sprite.getSprite(), pos.x * TILE_SIZE, pos.y * TILE_SIZE);
    }

    // Get all the upgradeable tiles
    auto upgradeView = registry.view<TilePosition, Upgrade>();
    for(auto &entity : upgradeView) {
        TilePosition &pos = upgradeView.get<TilePosition>(entity);
        Upgrade &upgrade = upgradeView.get<Upgrade>(entity);
        renderer.drawText(const_cast<char*>(std::to_string(upgrade.level).c_str()), pos.x * TILE_SIZE, pos.y * TILE_SIZE, 0xFFFFFF);
    }

    // Draw the map border
    renderer.drawBox(0, 0, map.getWidth() * TILE_SIZE, map.getHeight() * TILE_SIZE, 0x0000FF, false);
}

void GameTasks::renderTowerTargetConnections(Renderer &renderer, entt::registry &registry) {
    // Get all tiles with tower data associated with them (= towers)
    auto towerView = registry.view<TilePosition, Tower>();
    // Get all enemy entities
    auto enemyView = registry.view<Position, Enemy>();

    // For each tower get the actual targets the tower is firing at and draw a line from the tower to the targets
    for (auto &tower : towerView) {
        TilePosition &pos = towerView.get<TilePosition>(tower);
        Tower &towerData = towerView.get<Tower>(tower);

        std::set<entt::entity> &targets = towerData.getActualTargets();
        for (auto &target : targets) {
            // Safety check to make sure that the targets are actually valid.
            // Since we are not getting them from the registry directly (they are from the tower data after all)
            // we need to check their validity
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

        double remainingHpPercentage = health.value / (double) health.maxHealth;

        // Draw the green part of the health meter that resembles the remaining HP
        renderer.drawPie(pos.x, pos.y, 5, -90, -90 + 360 * remainingHpPercentage, 0x00FF00, true);
        // Draw the red part of the health meter that resembles the lost HP
        renderer.drawPie(pos.x, pos.y, 4, -90 + 360 * remainingHpPercentage, -90, 0xFF0000, true);
    }
}

void GameTasks::renderHoveredRanges(Renderer &renderer, entt::registry &registry, Map &map) {
    // Get all tiles with a range attached to them
    auto rangeView = registry.view<TilePosition, Range>();

    // Get the game's InputHandler and try to lock the mutex
    if(auto inputOpt = Game::get().getInput().lock()) {
        // Extract the InputHandler from the std::optional
        auto &input = *inputOpt;
        // Determine if the mouse is currently pointing at a tile
        if(auto tileOpt = map.getMapTileAtScreenPos(input->getMouseX(), input->getMouseY(), renderer)) {
            // If the entity, that represents the tile, has a tile position and range, then draw its range
            if(registry.has<TilePosition, Range>(*tileOpt)) {
                TilePosition &pos = rangeView.get<TilePosition>(*tileOpt);
                Range &range = rangeView.get<Range>(*tileOpt);

                renderer.drawCircle(pos.x * TILE_SIZE + TILE_SIZE / 2, pos.y * TILE_SIZE + TILE_SIZE / 2, range.radius, 0xFFFF00, false);
            }
        }

    }
}

void GameTasks::renderHUD(GameState &state, entt::registry &registry) {
    Map &map = state.getMap();
    Wave wave = state.getWave();
    TilePosition nexusPos = map.getNexusPosition();
    // Get the entity representing the nexus
    auto nexus = map.getMapTile(nexusPos.x, nexusPos.y);
    // Get the nexus' health
    Health nexusHealth = registry.get<Health>(nexus);

    // Draw UI background Boxes
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

    // Build the string that holds the information about which block is selected and draw it to the screen
    std::string selectedBlock("Selected Block: ");
    selectedBlock.append(getName(state.getTileTypeToPlace()));
    selectedBlock.append(" (Cost ");
    selectedBlock.append(std::to_string(getCostForType(state.getTileTypeToPlace())));
    selectedBlock.append(")");
    tumDrawText(const_cast<char*>(selectedBlock.c_str()), 5, SCREEN_HEIGHT - 25, 0xFFFFFF);
}

void GameTasks::renderPath(Renderer &renderer, std::vector<TilePosition> &path) {
    // The thickness of the line of the path
    static const int thickness = 3;
    // If there is no path, don't try to render it
    if (path.empty()) return;
    // The function calculating the color of the path segment as RGB, here in hexadecimal for ease of calculation
    auto color = [path](int segmentIndex) {
        if(segmentIndex < path.size() / 2) { // In the first half of the part, the color changes linearly from red to yellow
            return 0xFF0000 + ((0xFF * segmentIndex / (path.size() / 2)) << 8);
        } else { // In the second half the color changes linearly from yellow to green
            segmentIndex -= path.size() / 2;
            return 0xFFFF00 - ((0xFF * segmentIndex / (path.size() / 2)) << 16);
        }
    };

    // Render each path segment
    for (int i = 0; i < path.size() - 1; ++i) {
        TilePosition &tp1 = path[i];
        TilePosition &tp2 = path[i + 1];
        renderer.drawLine(
                tp1.x * TILE_SIZE + TILE_SIZE / 2,
                tp1.y * TILE_SIZE  + TILE_SIZE / 2,
                tp2.x * TILE_SIZE + TILE_SIZE / 2,
                tp2.y * TILE_SIZE + TILE_SIZE / 2,
                thickness,
                color(i));
    }
}
