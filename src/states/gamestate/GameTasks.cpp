
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
#include "../../components/Upgrade.h"

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
        // Extract the game state from the void pointer
        GameState &state = *static_cast<GameState*>(statePointer);
        // Get the state's registry, locked behind a mutex
        auto &regMutex = state.getRegistry();
        Renderer &renderer = state.getRenderer();
        Map &map = state.getMap();
        Game &game = Game::get();

        auto lastWake = xTaskGetTickCount();

        while(true) {

            // If the draw signal is not given, do not render
            if(!game.getDrawSignal().lock()) {
                vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
                continue;
            }

            // Only render if the registry can be unlocked
            if(auto regOpt = regMutex.lock()) {
                auto &registry = *regOpt;

                // If the screen lock can't be unlocked (= the screen is used by something else), do not render
                if (!game.getScreenLock().lock(portMAX_DELAY)) {
                    vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
                    continue;
                }

                // Clear the screen, setting everything to the background color
                tumDrawClear(0x000000);


                renderMap(renderer, *registry, map);

                // Render the path the enemies will take, but only if there is no wave currently active
                if (state.getWave().isFinished()) {
                    renderPath(renderer, map.getPath());
                }

                renderEntities(renderer, *registry);
                renderHoveredRanges(renderer, *registry, map);
                renderHealth(renderer, *registry);
                renderTowerTargetConnections(renderer, *registry);
                renderHUD(state, *registry);

                // Unlock the screen and signal the swap buffer task to do its job
                game.getScreenLock().unlock();
                game.getSwapBufferSignal().unlock();
            }
            // Delay the task until the next frame
            vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
        }
    }

    void gameMoveTask(void *statePointer) {
        GameState &state = *static_cast<GameState*>(statePointer);
        auto &regMutex = state.getRegistry();
        Map &map = state.getMap();

        auto lastWake = xTaskGetTickCount();
        // When the tasks are initialized this allowed for the move task to run first
        MOVE_SIGNAL.unlock();

        static auto margin = TILE_SIZE / 2;

        while(true) {

            // Wait for the Signal to run the task
            if(!MOVE_SIGNAL.lock(portMAX_DELAY)) {
                vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
                continue;
            }

            // Lock the registry's mutex
            if(auto regOpt = regMutex.lock()) {
                // Extract the registry from the std::optional
                auto &registry = *regOpt;

                // Get all entities with a Position and Velocity
                auto view = registry->view<Position, Velocity>();

                // For each of these entities, add their velocity value to their Position to move them
                for (auto &entity : view) {
                    Position &pos = view.get<Position>(entity);
                    Velocity &vel = view.get<Velocity>(entity);
                    pos.x += vel.dx;
                    pos.y += vel.dy;

                    // If an entity moves out of the map, it is flagged for deletion
                    if(pos.x > map.getWidth() * TILE_SIZE + margin || pos.x < -margin || pos.y > map.getHeight() * TILE_SIZE + margin || pos.y < -margin) {
                        if(registry->valid(entity)) registry->emplace_or_replace<Delete>(entity);
                    }
                }

                // Signal the collision task to run
                COLLISION_SIGNAL.unlock();
            }
            // Delay the task until the next frame
            vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
        }
    }

    void gameCollisionTask(void *statePointer) {
        GameState &state = *static_cast<GameState*>(statePointer);
        auto &regMutex = state.getRegistry();

        auto lastWake = xTaskGetTickCount();

        while(true) {

            // Wait for the Signal to run the task
            if(!COLLISION_SIGNAL.lock(portMAX_DELAY)) {
                vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
                continue;
            }

            if(auto regOpt = regMutex.lock()) {
                auto &registry = *regOpt;

                // Refresh all collision data related to tiles as broad phase collision detection
                state.getCollisionTable().refreshTiles(*registry);

                auto entityView = registry->view<Position, Hitbox>(); // All entities with a position and a hitbox
                auto tileView = registry->view<TilePosition, Hitbox>(); // All tiles with a hitbox
                auto enemyView = registry->view<Enemy, Health, Position, Hitbox>(); // All enemies with health position and a hitbox

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

                            // If the tile's and the entity's hitbox are solid, displace the entity accordingly
                            if(tileHitbox.solid && entityHitbox.solid) {
                                auto displacementVec = *collision;
                                entityPos.x += displacementVec.x;
                                entityPos.y += displacementVec.y;
                            } else if (registry->has<TileTypeComponent>(tile) && registry->get<TileTypeComponent>(tile).type == NEXUS) {
                                // If an enemy reaches the goal, they are flagged for deletion
                                if(enemyView.contains(entity) && !registry->has<Delete>(entity)){
                                    registry->emplace<Delete>(entity);
                                    tumSoundPlaySample(nexus_hit);
                                    // Damage the nexus accordingly
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
            // Delay the task until the next frame
            vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
        }
    }

    void gameKeyboardInputTask(void *statePointer) {
        GameState &state = *static_cast<GameState*>(statePointer);
        // Get the state's registry locked behind a mutex
        auto &regMutex = state.getRegistry();
        Game &game = Game::get();

        auto lastWake = xTaskGetTickCount();

        while(true) {
            // Try locking the mutex
            if(auto regOpt = regMutex.lock()) {
                // Extract the registry from the optional
                auto &registry = *regOpt;

                // Get a view only containing the player (the other 2 components are just here for safety)
                auto view = registry->view<Position, Velocity, Player>();

                // Try locking the input handler
                if(auto inputOpt = game.getInput().lock()) {
                    // Extract the input handler from the std::optional
                    auto &input = *inputOpt;

                    // The only entity in the view is the player
                    for (auto &player : view) {
                        Velocity &vel = view.get<Velocity>(player);
                        // Set Horizontal speed if Left/A, or Right/D are pressed
                        if((input->keyPressed(SDL_SCANCODE_LEFT) || input->keyPressed(SDL_SCANCODE_A)) &&
                        !(input->keyPressed(SDL_SCANCODE_RIGHT) || input->keyPressed(SDL_SCANCODE_D))) {
                            vel.dx = -vel.getCurrentMaxSpeed();
                        } else if((input->keyPressed(SDL_SCANCODE_RIGHT) || input->keyPressed(SDL_SCANCODE_D)) &&
                        !(input->keyPressed(SDL_SCANCODE_LEFT) || input->keyPressed(SDL_SCANCODE_A))) {
                            vel.dx = vel.getCurrentMaxSpeed();
                        } else { // If nothing is pressed, stop horizontal movement
                            vel.dx = 0;
                        }

                        // Set Vertical speed if Left/A, or Right/D are pressed
                        if((input->keyPressed(SDL_SCANCODE_UP) || input->keyPressed(SDL_SCANCODE_W)) && (!input->keyPressed(
                                SDL_SCANCODE_DOWN) && !input->keyPressed(
                                SDL_SCANCODE_S))) {
                            vel.dy = -vel.getCurrentMaxSpeed();
                        } else if((input->keyPressed(SDL_SCANCODE_DOWN) || input->keyPressed(SDL_SCANCODE_S)) && (!input->keyPressed(
                                SDL_SCANCODE_UP) && !input->keyPressed(
                                SDL_SCANCODE_W))) {
                            vel.dy = vel.getCurrentMaxSpeed();
                        } else { // If nothing is pressed, stop vertical movement
                            vel.dy = 0;
                        }

                        // Make the player have the same speed in every direction
                        vel.normalize();
                        vel.dx *= vel.getCurrentMaxSpeed();
                        vel.dy *= vel.getCurrentMaxSpeed();

                        Renderer &renderer = state.getRenderer();
                        // Handle zooming in and zooming out with + and -
                        if(input->keyPressed(SDL_SCANCODE_KP_PLUS)) {
                            renderer.setScale(std::min(renderer.getScale() * 1.01, 5.0));
                        }

                        if(input->keyPressed(SDL_SCANCODE_KP_MINUS)) {
                            renderer.setScale(std::max(renderer.getScale() / 1.01, 0.5));
                        }

                        // Center the camera on the player
                        Position &pos = view.get<Position>(player);
                        renderer.setOffset((SCREEN_WIDTH / 2) / renderer.getScale() - pos.x - PLAYER_SIZE / 2, (SCREEN_HEIGHT / 2) / renderer.getScale() - pos.y - PLAYER_SIZE / 2);
                    }
                }
            }
            // Delay the task until the next frame
            vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
        }
    }

    void gameMouseInputTask(void *statePointer) {
        GameState &state = *static_cast<GameState*>(statePointer);
        auto &regMutex = state.getRegistry();
        Game &game = Game::get();
        Map &map = state.getMap();

        auto lastWake = xTaskGetTickCount();
        auto &renderer = state.getRenderer();

        while(true) {
            // Try locking the mutex
            if(auto regOpt = regMutex.lock()) {
                // Extract the registry from the optional
                auto &registry = *regOpt;

                // Try locking the input handler
                if(auto inputOpt = game.getInput().lock()) {
                    // Extract the input handler from the std::optional
                    auto &input = *inputOpt;

                    if(input->leftClickDown()) {
                        // Check if the mouse is currently pointing at a tile
                        if (auto tileOpt = map.getMapTileAtScreenPos(input->getMouseX(), input->getMouseY(),renderer)) {
                            // Get the type of the tile the mouse is pointing at and the tile to be placed
                            TileType tileType = map.getTileType(*tileOpt, *registry);
                            TileType typeToPlace = state.getTileTypeToPlace();

                            // If the tile has a Damage and Upgrade component (which only towers have both)
                            if(registry->has<Damage, Upgrade>(*tileOpt)) {
                                Upgrade &upgrade = registry->get<Upgrade>(*tileOpt);
                                auto cost = levelUpCost(tileType, upgrade.level + 1);
                                // If the player can afford the cost, upgrade the tower and scale the damage accordingly
                                if(cost <= state.getCoins()) {
                                    Damage &damage = registry->get<Damage>(*tileOpt);
                                    upgrade.levelUp();
                                    upgrade.scaleDamage(damage);
                                    state.removeCoins(cost);
                                }
                            } else  {
                                // Calculate the cost of placing down the specific tile
                                int totalCost = getCostForType(typeToPlace) - getCostForType(tileType);
                                if (state.getWave().isFinished() && typeToPlace != EMPTY && state.getCoins() >= totalCost && !isSpecial(tileType)) {
                                    map.updateTileAtScreenPos(input->getMouseX(), input->getMouseY(), *registry, typeToPlace, renderer);
                                    // Pay the cost for the operation
                                    state.removeCoins(totalCost);
                                    tumSoundPlaySample(place);
                                    // If the tile was empty before, we need to update the enemy path, as it might have to change
                                    if (tileType == EMPTY) state.getMap().updateEnemyPath(*registry);
                                }
                            }
                            /*// Calculate the cost of placing down the specific tile
                            int totalCost = getCostForType(typeToPlace) - getCostForType(tileType);
                            // You can only place tiles on empty space while a wave isn't active. Also you have to have enough coins
                            // In addition, you cannot build on special tiles like the nexus or the enemy spawn
                            if((tileType != EMPTY || state.getWave().isFinished()) && tileType != typeToPlace && state.getCoins() >= totalCost && !isSpecial(tileType)) {
                                map.updateTileAtScreenPos(input->getMouseX(), input->getMouseY(), *registry, typeToPlace, renderer);
                                // Pay the cost for the operation
                                state.removeCoins(totalCost);
                                tumSoundPlaySample(place);
                                // If the tile was empty before, we need to update the enemy path, as it might have to change
                                if (tileType == EMPTY) state.getMap().updateEnemyPath(*registry);
                            }*/


                        }

                    // Check if a right click has happened and there is no wave active
                    } else if(input->rightClickDown() && state.getWave().isFinished()){
                        // Check if the mouse is currently pointing at a tile
                        if (auto tileOpt = map.getMapTileAtScreenPos(input->getMouseX(), input->getMouseY(),renderer)) {
                            // Get the type of the tile the mouse is pointing at
                            TileType tileType = map.getTileType(*tileOpt, *registry);

                            // You can't remove special tiles (spawn and nexus) and trying to remove empty tiles is redundant
                            if(!isSpecial(tileType) && tileType != EMPTY) {
                                // Refund the cost of the removed tile
                                if(registry->has<Upgrade>(*tileOpt)){
                                    // If the tile is upgradable get the cost of the upgraded tower
                                    Upgrade &upgrade = registry->get<Upgrade>(*tileOpt);
                                    state.addCoins(costAtLevel(tileType, upgrade.level));
                                } else {
                                    // If not, just refund the cost of the actual tile type
                                    state.addCoins(getCostForType(tileType));
                                }

                                // Remove the tile
                                map.updateTileAtScreenPos(input->getMouseX(), input->getMouseY(), *registry, EMPTY,renderer);

                                // Removing a tile might change the enemies' path, so it must be recalculated
                                state.getMap().updateEnemyPath(*registry);
                                tumSoundPlaySample(remove_tile);
                            }
                        }
                    }

                    // Check for one of the keys, tasked with selecting a tile to place, have been pressed
                    // If so, set the type to place, to that type and exit the loop
                    for (auto const& [scancode, tileType] : getScancodeMap()) {
                        if(input->keyPressed(scancode)) {
                            state.setTileTypeToPlace(tileType);
                            break;
                        }
                    }

                }
            }
            // Delay the task until the next frame
            vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
        }
    }

    void gameAITask(void *statePointer) {
        GameState &state = *static_cast<GameState*>(statePointer);
        auto &regMutex = state.getRegistry();
        auto lastWake = xTaskGetTickCount();

        while(true) {
            // Try locking the mutex
            if(auto regOpt = regMutex.lock()) {
                // Extract the registry from the std::optional
                auto &registry = *regOpt;

                auto view = registry->view<AIComponent>();
                auto deleteView = registry->view<Delete>();

                for (auto &entity : view) {
                    // Only execute AI action if the entity is not flagged for deletion
                    if(!deleteView.contains(entity)) {
                        AIComponent &ai = view.get<AIComponent>(entity);
                        // Execute a step of the entity's AI
                        ai.getAI()->act(*registry);
                    }
                }
            }
            // Delay the task until the next frame
            vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
        }
    }

    void gameSpawnTask(void *statePointer) {
        GameState &state = *static_cast<GameState*>(statePointer);
        auto &regMutex = state.getRegistry();
        auto lastWake = xTaskGetTickCount();

        // Initialize a random number generator
        srand((unsigned) time(NULL));

        while(true) {
            // Try locking the mutex
            if(auto regOpt = regMutex.lock()) {
                // Extract the registry from the std::optional
                auto &registry = *regOpt;
                Wave &wave = state.getWave();
                // If there are enemies left to spawn, spawn an enemy, give them an AI, that walks the path from the spawn to the map
                if (wave.getRemainingSpawns() > 0){
                    auto enemy = spawnEnemy(state.getMap().getSpawnPosition(), *registry, ENEMY_BASE_HEALTH * state.getWave().getEnemyHealthFactor());
                    registry->emplace<AIComponent>(enemy, new WalkPathAI(enemy, state.getMap().getPath()));
                    tumSoundPlaySample(enemy_spawn);
                    wave.decrementRemainingSpawns();
                }
            }
            // A random number between 0.4 and 1
            auto delayFactor = (double) rand() / RAND_MAX * 0.6 + 0.4;
            // Delay the task by "delayFactor" seconds
            vTaskDelayUntil(&lastWake, FRAME_TIME_MS * TARGET_FPS * delayFactor);
        }
    }

    void gameTowerTask(void *statePointer) {
        GameState &state = *static_cast<GameState*>(statePointer);
        auto &regMutex = state.getRegistry();
        auto lastWake = xTaskGetTickCount();

        while(true) {
            // Try locking the mutex
            if(auto regOpt = regMutex.lock()) {
                // Extract the registry from the std::optional
                auto &registry = *regOpt;

                auto enemyView = registry->view<Enemy, Position, Hitbox>();
                auto towerView = registry->view<TilePosition, Range, Tower>();

                // Refresh the data in the Collision table related to ranges for a broad phase collision detection
                state.getCollisionTable().refreshRanges(*registry);


                for(auto tower : towerView) {
                    // Get entities that are approximately in the tower's range
                    auto approxInRange = state.getCollisionTable().getEnemiesInRangeApprox(tower);
                    // If there aren't any potential targets even remotely in range, just continue with the next tower
                    if(approxInRange.empty()) continue;

                    // A set to hold potential targets of the tower
                    std::set<entt::entity> targets;
                    TilePosition &towerTilePos = towerView.get<TilePosition>(tower);
                    // The position of the center point of the tower
                    Position towerPos = towerTilePos.toPosition() + Position{TILE_SIZE / 2, TILE_SIZE / 2};
                    Range &towerRange = towerView.get<Range>(tower);
                    Tower &towerData = towerView.get<Tower>(tower);

                    // For each enemy that is potentially in range, do an exact collision detection with the range of the tower
                    // If the range and the enemy's hitbox intersect, add them to the targets set
                    for (auto &enemy : approxInRange) {
                        Position &enemyPos = enemyView.get<Position>(enemy);
                        Hitbox &enemyHitbox = enemyView.get<Hitbox>(enemy);

                        if(intersectHitboxRange(towerPos, towerRange, enemyPos, enemyHitbox)) {
                            targets.insert(enemy);
                        }
                    }

                    // Set the determined enemies as the potential targets of the tower
                    towerData.setPotentialTargets(targets);
                }
            }
            // Delay the task until the next frame
            vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
        }
    }

    void gameKillTask(void *statePointer) {
        GameState &state = *static_cast<GameState*>(statePointer);
        auto &regMutex = state.getRegistry();
        auto lastWake = xTaskGetTickCount();

        while(true) {
            // Try locking the mutex
            if(auto regOpt = regMutex.lock()) {
                // Extract the registry from the std::optional
                auto &registry = *regOpt;

                auto view = registry->view<Health>();

                // If an entity's health drops to or below 0, flag it for deletion
                for (auto &entity : view) {
                    Health &health = view.get<Health>(entity);
                    if(health.value <= 0) {
                        registry->emplace_or_replace<Delete>(entity);
                        // If said entity is an enemy, then reward the appropriate coins
                        if(registry->has<Enemy>(entity)){
                            state.addCoins(state.getWave().getEnemyCoins());
                            tumSoundPlaySample(enemy_death);
                        }

                    }
                }
            }
            // Delay the task until the next frame
            vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
        }
    }

    void gameWaveTask(void *statePointer) {
        GameState &state = *static_cast<GameState*>(statePointer);
        auto lastWake = xTaskGetTickCount();
        Game &game = Game::get();

        while(true) {
            Wave &wave = state.getWave();
            // If the wave is finished try locking the registry and input
            if(wave.isFinished()){
                // Try locking the input handler
                if(auto inputOpt = game.getInput().lock()) {
                    // Extract the input handler from the std::optional
                    auto &input = *inputOpt;
                    // If there is a path to the goal, and space is pressed, start a new wave and update
                    if (!state.getMap().getPath().empty() && input->keyPressed(SDL_SCANCODE_SPACE)) {
                        state.setWave(state.getWave().next());
                    }
                }
            }
            // Delay the task until the next frame
            vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
        }
    }

    void gameDeleteTask(void *statePointer) {
        GameState &state = *static_cast<GameState*>(statePointer);
        auto &regMutex = state.getRegistry();
        auto lastWake = xTaskGetTickCount();

        while(true) {
            // Try locking the mutex
            if(auto regOpt = regMutex.lock()) {
                // Extract the registry from the std::optional
                auto &registry = *regOpt;

                auto view = registry->view<Delete>();
                auto enemyView = registry->view<Enemy>();
                auto nexusView = registry->view<Nexus>();

                // This boolean is used to determine if the nexus has been destroyed. If so, this triggers the game over state
                bool gameOver = false;

                // The entities to delete. These cannot be deleted in the for loop, as that would mess with the view
                std::vector<entt::entity> toDelete;

                for (auto &entity : view) {
                    // If the deleted entity is an enemy, decrement the amount of enemies still alive
                    if(enemyView.contains(entity)) {
                        state.getWave().decrementRemainingEnemies();
                    }
                    // If the nexus is to be deleted, set the game over boolean to true
                    if(nexusView.contains(entity)) {
                        gameOver = true;
                    }

                    // Put the entity to be deleted in the set.
                    toDelete.push_back(entity);
                }

                // Delete the entities
                for (auto &entity : toDelete) {
                    registry->destroy(entity);
                }

                // If a game over occurs, the game over state is pushed to the stack
                if(gameOver) {
                    tumSoundPlaySample(game_over);
                    Game::get().enqueueStatePush(new GameOverState());
                }
            }
            // Delay the task until the next frame
            vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
        }
    }

}


