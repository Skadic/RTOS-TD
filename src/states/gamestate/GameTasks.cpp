
#include "GameState.h"
#include "../../Game.h"
#include "../../util/Log.h"
#include "../../components/Position.h"
#include "../../util/GlobalConsts.h"
#include "../../components/Hitbox.h"
#include "../../components/Health.h"
#include "../../components/tilecomponents/Tower.h"
#include "../../components/Enemy.h"
#include "../../components/Velocity.h"
#include "../../components/tilecomponents/TileTypeComponent.h"
#include "../../components/tags/Player.h"
#include "../../util/spawn/EntitySpawn.h"
#include "../../components/AI/WalkPathAI.h"
#include "../../util/RenderUtils.h"
#include "GameTasks.h"
#include "RenderTasks.h"
#include "../../components/tags/Projectile.h"
#include "../../components/Damage.h"
#include "../../components/tags/Delete.h"
#include "../../components/tags/Nexus.h"
#include "../gameoverstate/GameOverState.h"

extern "C" {
    #include <TUM_Sound.h>
}

#include <time.h>
#include <iostream>
#include <sstream>
#include <chrono>

/**
 * Used to synchronize running the collision task and movement task
 */
static Semaphore COLLISION_SIGNAL = Semaphore{xSemaphoreCreateBinary()};
static Semaphore MOVE_SIGNAL = Semaphore{xSemaphoreCreateBinary()};



namespace GameTasks {


    void gameRenderTask(void *statePointer) {

        GameState &state = *static_cast<GameState*>(statePointer);
        auto &regMutex = state.getRegistry();
        Game &game = Game::get();
        Renderer &renderer = state.getRenderer();
        Map &map = state.getMap();

        auto lastWake = xTaskGetTickCount();

        while(true) {
            logCurrentTaskName();
            if(game.getDrawSignal().lock(portMAX_DELAY)) {
                if(auto regOpt = regMutex.lock()) {
                    auto &registry = *regOpt;

                    if(game.getScreenLock().lock(portMAX_DELAY)){
                        tumDrawClear(INGAME_BG_COLOR);

                        if(state.getWave().isFinished()) {
                            renderPath(renderer, map.getPath());
                        }

                        renderMap(renderer, *registry, map);
                        renderEntities(renderer, *registry);
                        //renderRanges(renderer, *registry);

                        renderHoveredRanges(renderer, *registry, map);
                        renderHealth(renderer, *registry);
                        renderTowerTargetConnections(renderer, *registry);
                        renderHUD(state, *registry);

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
        Map &map = state.getMap();

        auto lastWake = xTaskGetTickCount();
        MOVE_SIGNAL.unlock();

        static auto margin = TILE_SIZE / 2;

        while(true) {
            logCurrentTaskName();

            // Wait for the Signal to run the task
            if(!MOVE_SIGNAL.lock(portMAX_DELAY)) {
                vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
                continue;
            }

            if(auto regOpt = regMutex.lock()) {
                auto &registry = *regOpt;

                auto view = registry->view<Position, Velocity>();

                for (auto &entity : view) {
                    Position &pos = view.get<Position>(entity);
                    Velocity &vel = view.get<Velocity>(entity);
                    pos.x += vel.dx;
                    pos.y += vel.dy;

                    if(pos.x > map.getWidth() * TILE_SIZE + margin || pos.x < -margin || pos.y > map.getHeight() * TILE_SIZE + margin || pos.y < -margin) {
                        if(registry->valid(entity)) registry->emplace_or_replace<Delete>(entity);
                    }
                }

                // Signal the collision task to run
                COLLISION_SIGNAL.unlock();
            }
            vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
        }
    }

    void gameCollisionTaskOld(void *statePointer) {
        GameState &state = *static_cast<GameState*>(statePointer);
        auto &regMutex = state.getRegistry();

        auto lastWake = xTaskGetTickCount();

        while(true) {
            logCurrentTaskName();

            // Wait for the Signal to run the task
            if(!COLLISION_SIGNAL.lock(portMAX_DELAY)) {
                vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
                continue;
            }

            if(auto regOpt = regMutex.lock()) {
                auto &registry = *regOpt;

                // Handle Collision with the map and displace Entities if needed.
                auto entityView = registry->view<Position, Hitbox, Velocity>();
                auto tileView = registry->view<TilePosition, Hitbox, TileTypeComponent>();

                for (auto &entity : entityView) {
                    for (auto &tile : tileView) {
                        Position &entityPos = entityView.get<Position>(entity);
                        Hitbox &entityHitbox = entityView.get<Hitbox>(entity);
                        TilePosition &tilePos = tileView.get<TilePosition>(tile);
                        Hitbox &tileHitbox = tileView.get<Hitbox>(tile);

                        if(auto collision = intersectHitbox(tilePos, tileHitbox, entityPos, entityHitbox)) {
                            auto type = registry->get<TileTypeComponent>(tile).type;

                            if(tileHitbox.solid && entityHitbox.solid) {
                                auto displacementVec = *collision;
                                entityPos.x += displacementVec.x;
                                entityPos.y += displacementVec.y;
                            } else if (type == GOAL) {
                                // If an enemy reaches the goal, they are flagged for deletion
                                if(registry->has<Enemy>(entity) && registry->has<Health>(entity)){
                                    registry->emplace<Delete>(entity);
                                    registry->remove<Enemy>(entity);
                                    Health &nexusHealth = registry->get<Health>(tile);
                                    nexusHealth.value--;
                                }

                            }
                        }
                    }
                }

                // Signal the move task to run
                MOVE_SIGNAL.unlock();
            }

            vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
        }
    }

    void gameCollisionTask(void *statePointer) {
        GameState &state = *static_cast<GameState*>(statePointer);
        auto &regMutex = state.getRegistry();

        auto lastWake = xTaskGetTickCount();

        while(true) {
            logCurrentTaskName();

            // Wait for the Signal to run the task
            if(!COLLISION_SIGNAL.lock(portMAX_DELAY)) {
                vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
                continue;
            }

            if(auto regOpt = regMutex.lock()) {
                auto &registry = *regOpt;

                // Refresh all collision data as broad phase collision detection
                state.getCollisionTable().refreshTiles(*registry);

                // Handle Collision with the map and displace Entities if needed.
                auto entityView = registry->view<Position, Hitbox>();
                auto tileView = registry->view<TilePosition, Hitbox>();
                auto enemyView = registry->view<Enemy, Health, Position, Hitbox>();

                for (auto &tile : tileView) {
                    TilePosition &tilePos = tileView.get<TilePosition>(tile);
                    Hitbox &tileHitbox = tileView.get<Hitbox>(tile);
                    // Get all entities, that are intersecting with the current tile
                    auto &intersecting = state.getCollisionTable().getIntersectingEntities(tilePos);

                    // Check the collision with this tile for each of these entities
                    for (auto &entity : intersecting) {
                        Position &entityPos = entityView.get<Position>(entity);
                        Hitbox &entityHitbox = entityView.get<Hitbox>(entity);

                        // Handle collision of entities with Tiles
                        if(auto collision = intersectHitbox(tilePos, tileHitbox, entityPos, entityHitbox)) {


                            if(tileHitbox.solid && entityHitbox.solid) {
                                auto displacementVec = *collision;
                                entityPos.x += displacementVec.x;
                                entityPos.y += displacementVec.y;
                            } else if (registry->has<TileTypeComponent>(tile) && registry->get<TileTypeComponent>(tile).type == GOAL) {
                                // If an enemy reaches the goal, they are flagged for deletion
                                if(enemyView.contains(entity) && !registry->has<Delete>(entity)){
                                    registry->emplace<Delete>(entity);
                                    tumSoundPlaySample(nexus_hit);
                                    Health &nexusHealth = registry->get<Health>(tile);
                                    nexusHealth.value--;
                                }
                            }
                        }
                    }
                }

                // Signal the Move task to run
                MOVE_SIGNAL.unlock();
            }

            vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
        }
    }

    void gameKeyboardInputTask(void *statePointer) {
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
                        if((input->buttonPressed(SDL_SCANCODE_LEFT) || input->buttonPressed(SDL_SCANCODE_A)) && (!input->buttonPressed(SDL_SCANCODE_RIGHT) && !input->buttonPressed(SDL_SCANCODE_D))) {
                            vel.dx = -vel.getCurrentMaxSpeed();
                        } else if((input->buttonPressed(SDL_SCANCODE_RIGHT) || input->buttonPressed(SDL_SCANCODE_D)) && (!input->buttonPressed(SDL_SCANCODE_LEFT) && !input->buttonPressed(SDL_SCANCODE_A))) {
                            vel.dx = vel.getCurrentMaxSpeed();
                        } else {
                            vel.dx = 0;
                        }

                        // Set Vertical speed
                        if((input->buttonPressed(SDL_SCANCODE_UP) || input->buttonPressed(SDL_SCANCODE_W)) && (!input->buttonPressed(SDL_SCANCODE_DOWN) && !input->buttonPressed(SDL_SCANCODE_S))) {
                            vel.dy = -vel.getCurrentMaxSpeed();
                        } else if((input->buttonPressed(SDL_SCANCODE_DOWN) || input->buttonPressed(SDL_SCANCODE_S)) && (!input->buttonPressed(SDL_SCANCODE_UP) && !input->buttonPressed(SDL_SCANCODE_W))) {
                            vel.dy = vel.getCurrentMaxSpeed();
                        } else {
                            vel.dy = 0;
                        }

                        // Make the player have the same speed in every direction
                        vel.normalize();
                        vel.dx *= vel.getCurrentMaxSpeed();
                        vel.dy *= vel.getCurrentMaxSpeed();

                        auto &renderer = state.getRenderer();


                        // Handle zooming in and zooming out with + and -
                        if(input->buttonPressed(SDL_SCANCODE_KP_PLUS)) {
                            renderer.setScale(std::min(renderer.getScale() * 1.01, 5.0));
                        }

                        if(input->buttonPressed(SDL_SCANCODE_KP_MINUS)) {
                            renderer.setScale(std::max(renderer.getScale() / 1.01, 0.5));
                        }

                        Position &pos = view.get<Position>(entity);
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
        Map &map = state.getMap();

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
                            TileType tileType = map.getTileType(*tileOpt, *registry);
                            TileType typeToPlace = state.getTileTypeToPlace();
                            int totalCost = getCostForType(typeToPlace) - getCostForType(tileType);
                            if((tileType != EMPTY || state.getWave().isFinished()) && tileType != typeToPlace && state.getCoins() >= totalCost && !isSpecial(tileType)) {
                                map.updateTileAtScreenPos(input->getMouseX(), input->getMouseY(), *registry, typeToPlace, renderer);
                                state.removeCoins(totalCost);
                                tumSoundPlaySample(place);
                                state.getMap().updateEnemyPath(*registry);
                            }
                        }

                    } else if(input->rightClickDown() && state.getWave().isFinished()){
                        const std::optional<entt::entity> &tileOpt =
                                map.getMapTileAtScreenPos(input->getMouseX(), input->getMouseY(),renderer);
                        if (tileOpt) {
                            TileType tileType = map.getTileType(*tileOpt, *registry);
                            if(!isSpecial(tileType) && tileType != EMPTY) {
                                map.updateTileAtScreenPos(input->getMouseX(), input->getMouseY(), *registry, EMPTY,renderer);
                                state.addCoins(getCostForType(tileType));
                                state.getMap().updateEnemyPath(*registry);
                                tumSoundPlaySample(remove_tile);
                            }
                        }
                    }

                    for (auto const& [scancode, tileType] : getScancodeMap()) {
                        if(input->buttonPressed(scancode)) {
                            state.setTileTypeToPlace(tileType);
                            break;
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
                auto deleteView = registry->view<Delete>();

                for (auto &entity : view) {
                    // Only execute AI action if the entity is not flagged for deletion
                    if(!deleteView.contains(entity)) {
                        AIComponent &ai = view.get<AIComponent>(entity);
                        ai.getAI()->act(*registry);
                    }
                }
            }

            vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
        }
    }

    void gameSpawnTask(void *statePointer) {
        GameState &state = *static_cast<GameState*>(statePointer);
        auto &regMutex = state.getRegistry();
        auto lastWake = xTaskGetTickCount();
        srand((unsigned) time(NULL));

        while(true) {
            logCurrentTaskName();
            if(auto regOpt = regMutex.lock()) {
                auto &registry = *regOpt;
                Wave &wave = state.getWave();
                if (wave.getRemainingSpawns() > 0){
                    auto enemy = spawnEnemy(state.getMap().getSpawnPosition(), *registry, ENEMY_BASE_HEALTH * state.getWave().getEnemyHealthFactor());
                    registry->emplace<AIComponent>(enemy, new WalkPathAI(enemy, state.getMap().getPath()));
                    tumSoundPlaySample(enemy_spawn);
                    wave.decreaseRemainingSpawns();
                }
            }
            auto delayFactor = (double) rand() / RAND_MAX * 0.6 + 0.4;
            vTaskDelayUntil(&lastWake, FRAME_TIME_MS * TARGET_FPS * delayFactor);
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

                state.getCollisionTable().refreshRanges(*registry);

                for(auto tower : towerView) {

                    auto approxInRange = state.getCollisionTable().getEnemiesInRangeApprox(tower);
                    // If there aren't any potentialTargets even remotely in range, just continue with the next tower
                    if(approxInRange.empty()) continue;

                    std::set<entt::entity> targets;
                    TilePosition &towerTilePos = towerView.get<TilePosition>(tower);
                    Position towerPos = towerTilePos.toPosition() + Position{TILE_SIZE / 2, TILE_SIZE / 2};
                    Range &towerRange = towerView.get<Range>(tower);
                    Tower &towerData = towerView.get<Tower>(tower);

                    for (auto &enemy : approxInRange) {
                        Position &enemyPos = enemyView.get<Position>(enemy);
                        Hitbox &enemyHitbox = enemyView.get<Hitbox>(enemy);

                        if(intersectHitboxRange(towerPos, towerRange, enemyPos, enemyHitbox)) {
                            targets.insert(enemy);
                        }
                    }

                    towerData.setPotentialTargets(targets);
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

                for (auto &entity : view) {
                    Health &health = view.get<Health>(entity);
                    if(health.value <= 0) {
                        registry->emplace_or_replace<Delete>(entity);
                        if(registry->has<Enemy>(entity)){
                            state.setCoins(state.getCoins()+state.getWave().getEnemyCoins());
                            tumSoundPlaySample(enemy_death);
                        }
                        //tumSoundPlaySample(enemy_death);
                    }
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
            Wave &wave = state.getWave();
            if(wave.isFinished()){
                if(auto regOpt = regMutex.lock()) {
                    auto &registry = *regOpt;
                    if(auto inputOpt = game.getInput().lock()) {
                        auto &input = *inputOpt;
                        if (!state.getMap().getPath().empty() && input->buttonPressed(SDL_SCANCODE_SPACE)) {
                            state.setWave(state.getWave().next());
                            state.getMap().updateEnemyPath(*registry);
                        }
                    }
                }
            }
            vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
        }
    }

    void gameDeleteTask(void *statePointer) {
        GameState &state = *static_cast<GameState*>(statePointer);
        auto &regMutex = state.getRegistry();
        auto lastWake = xTaskGetTickCount();

        while(true) {
            logCurrentTaskName();
            if(auto regOpt = regMutex.lock()) {
                auto &registry = *regOpt;

                auto view = registry->view<Delete>();
                auto enemyView = registry->view<Enemy>();
                auto nexusView = registry->view<Nexus>();


                bool gameOver = false;

                std::vector<entt::entity> toDelete;
                for (auto &entity : view) {
                    if(enemyView.contains(entity)) {
                        state.getWave().decrementRemainingEnemies();
                    }

                    if(nexusView.contains(entity)) {
                        gameOver = true;
                    }

                    toDelete.push_back(entity);
                }

                for (auto &entity : toDelete) {
                    registry->destroy(entity);
                }

                if(gameOver) {
                    tumSoundPlaySample(game_over);
                    Game::get().enqueueStatePush(new GameOverState());
                }
            }

            vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
        }
    }

}


