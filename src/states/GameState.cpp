
#include "GameState.h"
#include "../components/TilePosition.h"
#include "../components/Sprites.h"
#include "../components/Player.h"
#include "../util/GlobalConsts.h"
#include "../Game.h"
#include "../components/Position.h"
#include "../components/Velocity.h"
#include "../components/AI/AI.h"
#include "../components/AI/MoveTowardsPlayerAI.h"
#include "../util/Log.h"
#include "../util/TileType.h"
#include <iostream>
#include <string>
#include <SDL_scancode.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <map>


static std::shared_ptr<Sprite> ENEMY = std::make_shared<RectangleSprite>(PLAYER_SIZE, PLAYER_SIZE, 0xFF0000, true);
static std::shared_ptr<Sprite> PLAYER = std::make_shared<RectangleSprite>(PLAYER_SIZE, PLAYER_SIZE, 0x00FF00, true);

// v ------------------------------------- TASKS ------------------------------------- v

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
    Game &game = Game::get();

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

                short x = renderer.reverseTransformX(input->getMouseX()) / TILE_SIZE;
                short y = renderer.reverseTransformY(input->getMouseY()) / TILE_SIZE;

                if(input->leftClickDown()) {
                    if (auto tileOpt = state.getMap().getMapTileFromScreenPos(input->getMouseX(), input->getMouseY(), renderer)) {
                        auto view = registry->view<TilePosition, SpriteComponent>();
                        SpriteComponent &sprite = view.get<SpriteComponent>(*tileOpt);
                        sprite.setSprite(getSpriteForType(TOWER));
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
    Game &game = Game::get();

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

// ^ ------------------------------------- TASKS ------------------------------------- ^

GameState::GameState(int mapWidth, int mapHeight) :
        State(),
        map{Map(**registry.lock(portMAX_DELAY), mapWidth, mapHeight)},
        mapWidth{mapWidth},
        mapHeight{mapHeight} {
    renderer.setScale(2);

    initPlayer();
    initEnemy();

    addTask(gameRenderTask, "render", DEFAULT_TASK_STACK_SIZE, this, 0);
    addTask(gameMoveTask, "move", DEFAULT_TASK_STACK_SIZE, this, 0);
    addTask(gameControlPlayerTask, "control", DEFAULT_TASK_STACK_SIZE, this, 0);
    addTask(gameMouseInputTask, "mouse", DEFAULT_TASK_STACK_SIZE, this, 0);
    addTask(gameAITask, "ai", DEFAULT_TASK_STACK_SIZE, this, 0);

    suspendTasks();
}

GameState::GameState(std::string mapPath) :
        State(),
        map{Map(**registry.lock(portMAX_DELAY), mapPath)},
        mapWidth{mapWidth},
        mapHeight{mapHeight} {
    renderer.setScale(2);

    initPlayer();
    initEnemy();

    addTask(gameRenderTask, "render", DEFAULT_TASK_STACK_SIZE, this, 0);
    addTask(gameMoveTask, "move", DEFAULT_TASK_STACK_SIZE, this, 0);
    addTask(gameControlPlayerTask, "control", DEFAULT_TASK_STACK_SIZE, this, 0);
    addTask(gameMouseInputTask, "mouse", DEFAULT_TASK_STACK_SIZE, this, 0);
    addTask(gameAITask, "ai", DEFAULT_TASK_STACK_SIZE, this, 0);

    suspendTasks();
}

Map &GameState::getMap() {
    return map;
}

void GameState::initEnemy() {
    auto registry = *this->registry.lock();

    entt::entity enemy = registry->create();
    registry->emplace<Position>(enemy, 0, 0);
    registry->emplace<SpriteComponent>(enemy, ENEMY);
    registry->emplace<Velocity>(enemy, 0.0, 0.0);
    registry->emplace<AIComponent>(enemy, (AI*) new MoveTowardsPlayerAI(this, enemy));
}

void GameState::initPlayer() {
    auto registry = *this->registry.lock();

    entt::entity player = registry->create();
    registry->emplace<Player>(player);
    registry->emplace<SpriteComponent>(player, PLAYER);
    registry->emplace<Position>(player, SCREEN_WIDTH / 2 / renderer.getScale(), SCREEN_HEIGHT / 2 / renderer.getScale());
    registry->emplace<Velocity>(player, 0.0, 0.0);
}
