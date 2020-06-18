
#include "GameState.h"
#include "../components/TilePosition.h"
#include "../components/Sprites.h"
#include "../components/Player.h"
#include "../util/GlobalConsts.h"
#include "../Game.h"
#include "../components/Position.h"
#include "../components/Velocity.h"
#include <iostream>
#include <string>
#include <SDL_scancode.h>


static std::shared_ptr<Sprite> WALL = std::make_shared<RectangleSprite>(TILE_SIZE, TILE_SIZE, 0xFFFFFF, false);
static std::shared_ptr<Sprite> TOWER = std::make_shared<RectangleSprite>(TILE_SIZE, TILE_SIZE, 0x00FFFF, true);

static std::shared_ptr<Sprite> PLAYER = std::make_shared<RectangleSprite>(PLAYER_SIZE, PLAYER_SIZE, 0xFF0000, true);

void GameState::initMap() {
    auto registry = *this->registry.lock();

    map.resize(mapHeight);
    for (int y = 0; y < mapHeight; ++y) {
        map[y].resize(mapWidth);
        for (int x = 0; x < mapWidth; ++x) {
            entt::entity entity = registry->create();
            registry->emplace<TilePosition>(entity, x, y);
            registry->emplace<SpriteComponent>(entity, WALL);

            map[y][x] = entity;
        }
    }
}

void GameState::initPlayer() {
    auto registry = *this->registry.lock();

    entt::entity player = registry->create();
    registry->emplace<Player>(player);
    registry->emplace<Position>(player, SCREEN_WIDTH / 2 / renderer.getScale(), SCREEN_HEIGHT / 2 / renderer.getScale());
    registry->emplace<SpriteComponent>(player, PLAYER);
    registry->emplace<Velocity>(player, 0, 0);
}


// v ------------------------------------- TASKS ------------------------------------- v

void gameRenderTask(void *statePointer) {

    GameState &state = *static_cast<GameState*>(statePointer);
    auto &regMutex = state.getRegistry();
    Game &game = Game::get();

    auto lastWake = xTaskGetTickCount();

    while(true) {

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

        if(auto regOpt = regMutex.lock()) {
            auto &registry = *regOpt;

            auto view = registry->view<Position, Velocity, Player>();

            if(auto inputOpt = game.getInput().lock()) {
                auto &input = *inputOpt;

                for (auto &entity : view) {
                    Velocity &vel = view.get<Velocity>(entity);
                    // Set Horizontal speed

                     if(input->buttonPressed(SDL_SCANCODE_LEFT) && !input->buttonPressed(SDL_SCANCODE_RIGHT)) {
                        if(input->buttonPressed(SDL_SCANCODE_UP) && !input->buttonPressed(SDL_SCANCODE_DOWN)) {
                            vel.dx = -sqrt(PLAYER_SPEED*PLAYER_SPEED/2);
                            vel.dy = -sqrt(PLAYER_SPEED*PLAYER_SPEED/2);
                        } else if(!input->buttonPressed(SDL_SCANCODE_UP) && input->buttonPressed(SDL_SCANCODE_DOWN)) {
                            vel.dx = -sqrt(PLAYER_SPEED*PLAYER_SPEED/2);
                            vel.dy = sqrt(PLAYER_SPEED*PLAYER_SPEED/2);
                        } else {
                            vel.dx = -PLAYER_SPEED;
                        }
                    } else if(!input->buttonPressed(SDL_SCANCODE_LEFT) && input->buttonPressed(SDL_SCANCODE_RIGHT)) {
                        if(input->buttonPressed(SDL_SCANCODE_UP) && !input->buttonPressed(SDL_SCANCODE_DOWN)) {
                            vel.dx = sqrt(PLAYER_SPEED*PLAYER_SPEED/2);
                            vel.dy = -sqrt(PLAYER_SPEED*PLAYER_SPEED/2);
                        } else if(!input->buttonPressed(SDL_SCANCODE_UP) && input->buttonPressed(SDL_SCANCODE_DOWN)) {
                            vel.dx = sqrt(PLAYER_SPEED*PLAYER_SPEED/2);
                            vel.dy = sqrt(PLAYER_SPEED*PLAYER_SPEED/2);
                        } else {
                            vel.dx = PLAYER_SPEED;
                        }
                    } else {
                        vel.dx = 0;

                        if(input->buttonPressed(SDL_SCANCODE_UP) && !input->buttonPressed(SDL_SCANCODE_DOWN)) {
                            vel.dy = -PLAYER_SPEED;
                        } else if(!input->buttonPressed(SDL_SCANCODE_UP) && input->buttonPressed(SDL_SCANCODE_DOWN)) {
                            vel.dy = PLAYER_SPEED;
                        } else {
                            vel.dy = 0;
                        }
                    }


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

        if(auto regOpt = regMutex.lock()) {
            auto &registry = *regOpt;

            if(auto inputOpt = game.getInput().lock()) {
                auto &input = *inputOpt;
                auto &renderer = state.getRenderer();

                short x = renderer.reverseTransformX(input->getMouseX()) / TILE_SIZE;
                short y = renderer.reverseTransformY(input->getMouseY()) / TILE_SIZE;

                if(input->leftClicked()) {
                    if (auto tileOpt = state.getMapTileFromScreenPos(input->getMouseX(), input->getMouseY())) {
                        auto view = registry->view<TilePosition, SpriteComponent>();
                        SpriteComponent &sprite = view.get<SpriteComponent>(*tileOpt);
                        sprite.setSprite(TOWER);
                    }
                }
            }
        }

        vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
    }
}

// ^ ------------------------------------- TASKS ------------------------------------- ^

GameState::GameState(int mapWidth, int mapHeight) :
        State(),
        map{std::vector<std::vector<entt::entity>>()},
        mapWidth{mapWidth},
        mapHeight{mapHeight} {
    renderer.setScale(2);

    initMap();
    initPlayer();

    addTask(gameRenderTask, "render", DEFAULT_TASK_STACK_SIZE, this, 0);
    addTask(gameMoveTask, "move", DEFAULT_TASK_STACK_SIZE, this, 0);
    addTask(gameControlPlayerTask, "control", DEFAULT_TASK_STACK_SIZE, this, 0);
    addTask(gameMouseInputTask, "mouse", DEFAULT_TASK_STACK_SIZE, this, 0);


    suspendTasks();
}

entt::entity &GameState::getMapTile(int x, int y) {
    return map[y][x];
}

const int GameState::getMapWidth() const {
    return mapWidth;
}

const int GameState::getMapHeight() const {
    return mapHeight;
}

std::optional<entt::entity> GameState::getMapTileFromScreenPos(short xPos, short yPos) {
    short x = renderer.reverseTransformX(xPos) / TILE_SIZE;
    short y = renderer.reverseTransformY(yPos) / TILE_SIZE;

    if(x >= 0 && y >= 0 && x < getMapWidth() && y < getMapHeight()) {
        return std::make_optional<entt::entity>(getMapTile(x, y));
    } else {
        return std::optional<entt::entity>{};
    }
}
