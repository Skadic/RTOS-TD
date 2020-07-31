
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
#include "GameTasks.h"
#include "RenderTasks.h"
#include "../components/tags/Projectile.h"
#include "../components/Damage.h"
#include "../components/tags/Delete.h"

extern "C" {
    #include <TUM_Sound.h>
}

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
                        tumDrawClear(0x000000);


                        renderMap(renderer, *registry, map);
                        renderEntities(renderer, *registry);
                        //renderRanges(renderer, *registry);
                        renderHealth(renderer, *registry);
                        renderTowerTargetConnections(renderer, *registry);

                        Wave wave = state.getWave();
                        TilePosition nexusPos = map.getNexusPosition();
                        auto nexus = map.getMapTile(nexusPos.x, nexusPos.y);
                        Health nexusHealth = registry->get<Health>(nexus);

                        if (wave.isFinished()){
                            tumDrawText("BUILDING PHASE", (SCREEN_WIDTH/2)-55, 25, 0xFFFFFF);
                            drawInfo("Prepare yourself for Wave ", wave.getWaveNumber()+1, (SCREEN_WIDTH/2)-85, 5);
                        }else{
                            drawInfo("Wave: ", wave.getWaveNumber(), (SCREEN_WIDTH/2)-25, 5);
                            drawInfo("Enemies remaining: ", wave.getRemainingEnemies(),(SCREEN_WIDTH/2)-70, 25);
                            drawInfo("Coins per enemy: ", wave.getEnemyCoins(), SCREEN_WIDTH-155, SCREEN_HEIGHT-25);
                            drawInfo("Enemies health: ", 100*wave.getEnemyHealthFactor(),SCREEN_WIDTH-155, SCREEN_HEIGHT-45);
                        }
                        drawInfo("Nexus Health: ", nexusHealth.value, SCREEN_WIDTH-125, 5);
                        drawInfo("Coins: ", state.getCoins(), 5, 5);


                        std::string text("Selected Block: ");
                        text.append(getName(state.getTileTypeToPlace()));
                        tumDrawText(strdup(text.c_str()), 5, SCREEN_HEIGHT-25, 0xFFFFFF);

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

                    if(pos.x > map.getWidth() * TILE_SIZE || pos.x < 0 || pos.y > map.getHeight() * TILE_SIZE || pos.y < 0) {
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
                auto tileView = registry->view<TilePosition, Hitbox, TileTypeComponent>();
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
                            auto type = registry->get<TileTypeComponent>(tile).type;

                            if(tileHitbox.solid && entityHitbox.solid) {
                                auto displacementVec = *collision;
                                entityPos.x += displacementVec.x;
                                entityPos.y += displacementVec.y;
                            } else if (type == GOAL) {
                                // If an enemy reaches the goal, they are flagged for deletion
                                if(enemyView.contains(entity) && !registry->has<Delete>(entity)){
                                    registry->emplace<Delete>(entity);
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
                        if((input->buttonPressed(SDL_SCANCODE_LEFT) || input->buttonPressed(SDL_SCANCODE_A)) && (!input->buttonPressed(SDL_SCANCODE_RIGHT) && !input->buttonPressed(SDL_SCANCODE_D))) {
                            vel.dx = -PLAYER_SPEED;
                        } else if((input->buttonPressed(SDL_SCANCODE_RIGHT) || input->buttonPressed(SDL_SCANCODE_D)) && (!input->buttonPressed(SDL_SCANCODE_LEFT) && !input->buttonPressed(SDL_SCANCODE_A))) {
                            vel.dx = PLAYER_SPEED;
                        } else {
                            vel.dx = 0;
                        }

                        // Set Vertical speed
                        if((input->buttonPressed(SDL_SCANCODE_UP) || input->buttonPressed(SDL_SCANCODE_W)) && (!input->buttonPressed(SDL_SCANCODE_DOWN) && !input->buttonPressed(SDL_SCANCODE_S))) {
                            vel.dy = -PLAYER_SPEED;
                        } else if((input->buttonPressed(SDL_SCANCODE_DOWN) || input->buttonPressed(SDL_SCANCODE_S)) && (!input->buttonPressed(SDL_SCANCODE_UP) && !input->buttonPressed(SDL_SCANCODE_W))) {
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
                            if((tileType != EMPTY || state.getWave().isFinished()) && state.getCoins() >= totalCost && !isSpecial(tileType)) {
                                map.updateTileAtScreenPos(input->getMouseX(), input->getMouseY(), *registry, typeToPlace, renderer);
                                state.removeCoins(totalCost);
                            }
                        }

                    } else if(input->rightClickDown() && state.getWave().isFinished()){
                        const std::optional<entt::entity> &tileOpt =
                                map.getMapTileAtScreenPos(input->getMouseX(), input->getMouseY(),renderer);
                        if (tileOpt) {
                            TileType tileType = map.getTileType(*tileOpt, *registry);
                            if(!isSpecial(tileType)) {
                                map.updateTileAtScreenPos(input->getMouseX(), input->getMouseY(), *registry, EMPTY,renderer);
                                state.addCoins(getCostForType(tileType));
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
                        if (input->buttonPressed(SDL_SCANCODE_SPACE)) {
                            state.setWave(
                                    Wave(wave.getSpawnLimit() + 3, wave.getEnemyHealthFactor() * 1.2,
                                         wave.getEnemyCoins() + 1, wave.getWaveNumber() + 1));
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

                std::vector<entt::entity> toDelete;
                for (auto &entity : view) {
                    if(registry->has<Enemy>(entity)) {
                        state.getWave().decrementRemainingEnemies();
                    }
                    toDelete.push_back(entity);
                }

                for (auto &entity : toDelete) {
                    registry->destroy(entity);
                }
            }

            vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
        }
    }

}


