
#include "GameState.h"
#include "../Game.h"
#include "../util/Log.h"
#include "../components/Position.h"
#include "../util/GlobalConsts.h"
#include "../components/Hitbox.h"
#include "../components/Health.h"
#include "../components/tilecomponents/Tower.h"
#include "../components/Enemy.h"
#include "../components/Velocity.h"
#include "../components/tilecomponents/TileTypeComponent.h"
#include "../components/tags/Player.h"
#include "../util/spawn/EntitySpawn.h"
#include "../components/AI/PathfindToNexusAI.h"
#include "../util/StringUtils.h"
extern "C" {
    #include <TUM_Sound.h>
}
#include <iostream>
#include <sstream>


namespace GameTasks {
    void gameRenderTask(void *statePointer) {

        GameState &state = *static_cast<GameState*>(statePointer);
        auto &regMutex = state.getRegistry();
        Game &game = Game::get();

        auto lastWake = xTaskGetTickCount();

        while(true) {
            logCurrentTaskName();
            if(game.getDrawSignal().lock(portMAX_DELAY)) {
                if(auto regOpt = regMutex.lock()) {
                    auto &registry = *regOpt;

                    // Get all tile entities from the registry
                    auto tileView = registry->view<TilePosition, SpriteComponent>();
                    // Get all the other entities which need to be rendered
                    auto renderView = registry->view<Position, SpriteComponent>();

                    // Get all entities with a velocity and position
                    //auto velView = registry->view<Position, Velocity, SpriteComponent>();

                    if(game.getScreenLock().lock(portMAX_DELAY)){

                        tumDrawClear(0x000000);

                        // Render map
                        for(auto &entity : tileView) {
                            TilePosition &pos = tileView.get<TilePosition>(entity);
                            SpriteComponent &sprite = tileView.get<SpriteComponent>(entity);
                            state.getRenderer().drawSprite(*sprite.getSprite(), pos.x * TILE_SIZE, pos.y * TILE_SIZE);
                        }

                        state.getRenderer().drawBox(0, 0, state.getMap().getWidth() * TILE_SIZE, state.getMap().getHeight() * TILE_SIZE, 0x0000FF, false);

                        // Render all else
                        for(auto &entity : renderView) {
                            Position &pos = renderView.get<Position>(entity);
                            SpriteComponent &sprite = renderView.get<SpriteComponent>(entity);
                            state.getRenderer().drawSprite(*sprite.getSprite(), pos.x, pos.y);
                        }

                        auto rangeView = registry->view<TilePosition, SpriteComponent, Range>();

                        // Render Ranges
                        for (auto &entity : rangeView) {
                            TilePosition &pos = rangeView.get<TilePosition>(entity);
                            SpriteComponent &sprite = rangeView.get<SpriteComponent>(entity);
                            Range &range = rangeView.get<Range>(entity);

                            state.getRenderer().drawCircle(pos.x * TILE_SIZE + TILE_SIZE / 2, pos.y * TILE_SIZE + TILE_SIZE / 2, range.radius, 0xFFFF00, false);
                        }

                        auto healthView = registry->view<Position, Health>();

                        for (auto &entity : healthView) {
                            Position &pos = healthView.get<Position>(entity);
                            Health &health = healthView.get<Health>(entity);

                            state.getRenderer().drawPie(pos.x, pos.y, 5, -90, -90 + 360 * (health.value / (double) health.maxHealth), 0x00FF00, true);
                            state.getRenderer().drawPie(pos.x, pos.y, 4, -90 + 360 * (health.value / (double) health.maxHealth), -90, 0xFF0000, true);
                        }

                        auto towerView = registry->view<TilePosition, Tower>();
                        auto enemyView = registry->view<Position, Enemy>();

                        for (auto &tower : towerView) {
                            TilePosition &pos = towerView.get<TilePosition>(tower);
                            Tower &towerData = towerView.get<Tower>(tower);

                            std::set<entt::entity> &targets = towerData.getTargets();
                            for (auto &target : targets) {
                                if(registry->valid(target)) {
                                    Position &targetPos = enemyView.get<Position>(target);
                                    state.getRenderer().drawLine(pos.x * TILE_SIZE + TILE_SIZE / 2, pos.y * TILE_SIZE + TILE_SIZE / 2, targetPos.x + PLAYER_SIZE / 2, targetPos.y + PLAYER_SIZE / 2, 2, 0x800020);
                                }
                            }
                        }

                        Wave wave = state.getWave();
                        Map map = state.getMap();
                        TilePosition pos = state.getMap().getNexus();
                        auto nexus = map.getMapTile(pos.x, pos.y);
                        Health nexusHealth = registry->get<Health>(nexus);

                        drawInfo("Enemies remaining: ", wave.getRemainingEnemies(),5, 5);
                        drawInfo("Enemies health: ", 100*wave.getEnemyHealthFactor(),5, 25);
                        drawInfo("Wave: ", wave.getWaveNumber(), SCREEN_WIDTH-65, 5);
                        drawInfo("Nexus Health: ", nexusHealth.value, (SCREEN_WIDTH/2)-45, 5);
                        drawInfo("Coins: ", state.getCoins(), 5, SCREEN_HEIGHT-25);
                        drawInfo("Coins per enemy: ", wave.getEnemyCoins(), 5, SCREEN_HEIGHT-45);


                        game.getScreenLock().unlock();
                        game.getSwapBufferSignal().unlock();
                    }
                }
            }
            vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
        }
    }

    void gameMoveTask(void *statePointer) {
        GameState &state = *static_cast<GameState*>(statePointer);
        auto &regMutex = state.getRegistry();

        auto lastWake = xTaskGetTickCount();

        while(true) {

            if(auto regOpt = regMutex.lock()) {
                auto &registry = *regOpt;

                auto view = registry->view<Position, Velocity>();

                for (auto &entity : view) {
                    Position &pos = view.get<Position>(entity);
                    Velocity &vel = view.get<Velocity>(entity);
                    pos.x += vel.dx;
                    pos.y += vel.dy;
                }

                // Handle Collision with the map and displace Entities if needed.
                auto entityView = registry->view<Position, Hitbox, Velocity>();
                auto tileView = registry->view<TilePosition, Hitbox, TileTypeComponent>();

                // Contains all entities that should be deleted after reaching the goal
                // If we deleted the entities in the loop, we would mess up the views and get errors
                // So we just flag them as to be destroyed, and delete them afterwards
                std::vector<entt::entity> toDestroy;

                for (auto &entity : entityView) {
                    for (auto &tile : tileView) {
                        Position &entityPos = entityView.get<Position>(entity);
                        Hitbox &entityHitbox = entityView.get<Hitbox>(entity);
                        TilePosition &tilePos = tileView.get<TilePosition>(tile);
                        Hitbox &tileHitbox = tileView.get<Hitbox>(tile);

                        if(auto collision = intersectHitbox(tilePos, tileHitbox, entityPos, entityHitbox)) {
                            auto type = registry->get<TileTypeComponent>(tile).type;

                            if(isSolid(type)) {
                                auto displacementVec = *collision;
                                entityPos.x += displacementVec.x;
                                entityPos.y += displacementVec.y;
                            } else {
                                if (type == GOAL) {
                                    // If an enemy reaches the goal, they are flagged for deletion
                                    if(registry->has<Enemy>(entity) && registry->has<Health>(entity)){
                                        Health &health = registry->get<Health>(entity);
                                        health.value = 0;
                                        Health &nexusHealth = registry->get<Health>(tile);
                                        nexusHealth.value--;
                                        state.setCoins(state.getCoins()-state.getWave().getEnemyCoins());
                                    }


                                }
                            }
                        }
                    }
                }

                for(auto &entity : toDestroy) registry->destroy(entity);
            }

            vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
        }
    }

    void gameControlPlayerTask(void *statePointer) {
        GameState &state = *static_cast<GameState*>(statePointer);
        auto &regMutex = state.getRegistry();
        Game &game = Game::get();

        auto lastWake = xTaskGetTickCount();

        while(true) {
            logCurrentTaskName();
            if(auto regOpt = regMutex.lock()) {
                auto &registry = *regOpt;

                auto view = registry->view<Position, Velocity, Player>();

                if(auto inputOpt = game.getInput().lock()) {
                    auto &input = *inputOpt;

                    for (auto &entity : view) {
                        Velocity &vel = view.get<Velocity>(entity);
                        // Set Horizontal speed
                        if(input->buttonPressed(SDL_SCANCODE_LEFT) && !input->buttonPressed(SDL_SCANCODE_RIGHT)) {
                            vel.dx = -PLAYER_SPEED;
                        } else if(!input->buttonPressed(SDL_SCANCODE_LEFT) && input->buttonPressed(SDL_SCANCODE_RIGHT)) {
                            vel.dx = PLAYER_SPEED;
                        } else {
                            vel.dx = 0;
                        }

                        // Set Vertical speed
                        if(input->buttonPressed(SDL_SCANCODE_UP) && !input->buttonPressed(SDL_SCANCODE_DOWN)) {
                            vel.dy = -PLAYER_SPEED;
                        } else if(!input->buttonPressed(SDL_SCANCODE_UP) && input->buttonPressed(SDL_SCANCODE_DOWN)) {
                            vel.dy = PLAYER_SPEED;
                        } else {
                            vel.dy = 0;
                        }

                        // Make the player have the same speed in every direction
                        vel.normalize();
                        vel.dx *= PLAYER_SPEED;
                        vel.dy *= PLAYER_SPEED;

                        Position &pos = view.get<Position>(entity);
                        auto &renderer = state.getRenderer();
                        renderer.setOffset((SCREEN_WIDTH / 2) / renderer.getScale() - pos.x - PLAYER_SIZE / 2, (SCREEN_HEIGHT / 2) / renderer.getScale() - pos.y - PLAYER_SIZE / 2);
                    }
                }
            }

            vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
        }
    }

    void gameMouseInputTask(void *statePointer) {
        GameState &state = *static_cast<GameState*>(statePointer);
        auto &regMutex = state.getRegistry();
        Game &game = Game::get();

        auto lastWake = xTaskGetTickCount();

        while(true) {
            logCurrentTaskName();
            if(auto regOpt = regMutex.lock()) {
                auto &registry = *regOpt;

                if(auto inputOpt = game.getInput().lock()) {
                    auto &input = *inputOpt;
                    auto &renderer = state.getRenderer();

                    if(input->leftClickDown()) {
                        Map &map = state.getMap();
                        const std::optional<entt::entity> &tileOpt = map.getMapTileAtScreenPos(input->getMouseX(),
                                                                                               input->getMouseY(),
                                                                                               renderer);
                        if (tileOpt) {
                            if(map.getTileType(*tileOpt, *registry) == WALL && state.getCoins() >= 5) {
                                map.updateTileAtScreenPos(input->getMouseX(), input->getMouseY(), *registry, TOWER,
                                                          renderer);
                                state.setCoins(state.getCoins()-5);
                            } else if(map.getTileType(*tileOpt, *registry) == TOWER) {
                                map.updateTileAtScreenPos(input->getMouseX(), input->getMouseY(), *registry, WALL,
                                                          renderer);
                                state.setCoins(state.getCoins()+3);
                            }
                        }

                    }
                }
            }

            vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
        }
    }

    void gameAITask(void *statePointer) {
        GameState &state = *static_cast<GameState*>(statePointer);
        auto &regMutex = state.getRegistry();
        auto lastWake = xTaskGetTickCount();

        while(true) {
            logCurrentTaskName();
            if(auto regOpt = regMutex.lock()) {
                auto &registry = *regOpt;

                auto view = registry->view<AIComponent>();

                for (auto &entity : view) {
                    AIComponent &ai = view.get<AIComponent>(entity);
                    ai.getAI()->act(*registry);
                }
            }

            vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
        }
    }

    void gameSpawnTask(void *statePointer) {
        GameState &state = *static_cast<GameState*>(statePointer);
        auto &regMutex = state.getRegistry();
        auto lastWake = xTaskGetTickCount();

        while(true) {
            logCurrentTaskName();
            if(auto regOpt = regMutex.lock()) {
                auto &registry = *regOpt;
                Wave &wave = state.getWave();
                if (wave.getEnemyCount()>0){
                    auto enemy = spawnEnemy(state.getMap().getSpawn(), *registry, 100*state.getWave().getEnemyHealthFactor());
                    registry->emplace<AIComponent>(enemy, new PathfindToNexusAI(&state, enemy, state.getMap().getPath()));
                    //tumSoundPlaySample(enemy_spawn);
                    wave.decreaseEnemyCount();
                }
            }

            vTaskDelayUntil(&lastWake, FRAME_TIME_MS * TARGET_FPS);
        }
    }

    void gameTowerTask(void *statePointer) {
        GameState &state = *static_cast<GameState*>(statePointer);
        auto &regMutex = state.getRegistry();
        auto lastWake = xTaskGetTickCount();

        while(true) {
            logCurrentTaskName();
            if(auto regOpt = regMutex.lock()) {
                auto &registry = *regOpt;

                auto enemyView = registry->view<Enemy, Position, Hitbox>();
                auto towerView = registry->view<TilePosition, Range, Tower>();

                for(auto &tower : towerView) {
                    std::vector<entt::entity> targets;
                    TilePosition &towerTilePos = towerView.get<TilePosition>(tower);
                    Position towerPos = Position{static_cast<float>(towerTilePos.x * TILE_SIZE + TILE_SIZE / 2), static_cast<float>(towerTilePos.y * TILE_SIZE + TILE_SIZE / 2)};
                    Range &towerRange = towerView.get<Range>(tower);
                    Tower &towerData = towerView.get<Tower>(tower);

                    for (auto &enemy : enemyView) {
                        Position &enemyPos = enemyView.get<Position>(enemy);
                        Hitbox &enemyHitbox = enemyView.get<Hitbox>(enemy);

                        if(intersectHitboxRange(towerPos, towerRange, enemyPos, enemyHitbox)) {
                            targets.push_back(enemy);
                        }
                    }
                    towerData.setTargets(targets);
                }
            }

            vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
        }
    }

    void gameKillTask(void *statePointer) {
        GameState &state = *static_cast<GameState*>(statePointer);
        auto &regMutex = state.getRegistry();
        auto lastWake = xTaskGetTickCount();

        while(true) {
            logCurrentTaskName();
            if(auto regOpt = regMutex.lock()) {
                auto &registry = *regOpt;

                auto view = registry->view<Health>();

                std::vector<entt::entity> toDelete;
                for (auto &entity : view) {
                    Health &health = view.get<Health>(entity);
                    if(health.value <= 0) {
                        toDelete.push_back(entity);
                        if(registry->has<Enemy>(entity)){
                            state.getWave().setRemainingEnemies(state.getWave().getRemainingEnemies()-1);
                            state.setCoins(state.getCoins()+state.getWave().getEnemyCoins());
                        }
                        //tumSoundPlaySample(enemy_death);
                    }
                }

                for (auto &entity : toDelete) {
                    registry->destroy(entity);

                }
            }

            vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
        }
    }

    void gameWaveTask(void *statePointer) {
        GameState &state = *static_cast<GameState*>(statePointer);
        auto &regMutex = state.getRegistry();
        auto lastWake = xTaskGetTickCount();
        Game &game = Game::get();

        while(true) {
            logCurrentTaskName();
            if(state.getWave().getRemainingEnemies()==0){
                if(auto inputOpt = game.getInput().lock()) {
                    auto &input = *inputOpt;
                    if (input->buttonPressed(SDL_SCANCODE_SPACE)) {
                        state.setWave(
                                Wave(state.getWave().getSpawnLimit() + 3, state.getWave().getEnemyHealthFactor() * 1.5,
                                     state.getWave().getEnemyCoins() + 1, state.getWave().getWaveNumber() + 1));
                    }
                }
            }
            vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
        }
    }

}


