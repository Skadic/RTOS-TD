
#include "GameState.h"
#include "../components/TilePosition.h"
#include "../components/Sprites.h"
#include "../util/GlobalConsts.h"
#include "../Game.h"
#include <iostream>
#include <string>

GameState::GameState(int mapWidth, int mapHeight) :
    State(),
    map{std::vector<std::vector<entt::entity>>()},
    mapWidth{mapWidth},
    mapHeight{mapHeight} {

    initMap();

    TaskHandle_t renderMap = nullptr;
    xTaskCreate(renderMapTask, "render_map", DEFAULT_TASK_STACK_SIZE, this, 0, &renderMap);
    tasks.push_back(renderMap);
}

void GameState::initMap() {
    auto registry = *this->registry.lock();

    map.resize(mapHeight);
    for (int y = 0; y < mapHeight; ++y) {
        map[y].resize(mapWidth);
        for (int x = 0; x < mapWidth; ++x) {
            entt::entity entity = registry->create();
            registry->emplace<TilePosition>(entity, x, y);
            registry->emplace<SpriteComponent>(entity, new RectangleSprite{TILE_SIZE, TILE_SIZE, 0xFFFFFF, false});

            map[y][x] = entity;
        }
    }
}


void renderMapTask(void *statePointer) {

    GameState &state = *static_cast<GameState*>(statePointer);
    auto &regMutex = state.getRegistry();

    Game &game = Game::get();

    auto lastWake = xTaskGetTickCount();

    while(true) {

        if(game.getDrawSignal().lock(portMAX_DELAY)) {
            if(auto regOpt = regMutex.lock()) {
                auto &registry = *regOpt;
                auto view = registry->view<TilePosition, SpriteComponent>();

                if(game.getScreenLock().lock(portMAX_DELAY)){

                    tumDrawClear(0x000000);

                    for(auto &entity : view) {
                        TilePosition &pos = view.get<TilePosition>(entity);
                        SpriteComponent &sprite = view.get<SpriteComponent>(entity);
                        sprite.getSprite()->draw(pos.x * TILE_SIZE, pos.y * TILE_SIZE);
                    }

                    game.getScreenLock().unlock();
                }
            }
        }
        vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
    }
}
