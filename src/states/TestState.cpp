
#include "TestState.h"

#include "../components/Components.h"
#include "../Game.h"
#include <cmath>
#include <thread>

#include <iostream>

extern "C" {
#include <TUM_Draw.h>
#include <semphr.h>
}

inline const float MOVE_SPEED = 300;

// The task that should render the Rectangles to the screen
void renderTask(void *registryPointer) {
    //std::cout << "Thread[" << std::this_thread::get_id() << "] running task: " << pcTaskGetName(xTaskGetCurrentTaskHandle()) << std::endl;
    Game& game = Game::get();
    auto &regMutex = game.getRegistry();

    // The last time this task was waked by the scheduler
    auto lastWake = xTaskGetTickCount();

    while (true) {
        std::cout << "render" << std::endl;
        // Try taking the Draw Signal semaphore. The task should only draw if a draw signal is given by the swap_buffer task
        if(xSemaphoreTake(game.getDrawSignal(), portMAX_DELAY) == pdTRUE) {
            if(auto regOpt = regMutex.lock()) {
                Guard<entt::registry> &registry = *regOpt;
                // Get all entities with a Position and a Rectangle Sprite component
                auto view = registry->view<Position, SpriteComponent>();

                // Takes the screenLock Semaphore to have exclusive access to drawing
                xSemaphoreTake(game.getScreenLock(), portMAX_DELAY);
                // Clears the screen to black
                tumDrawClear(0x000000);

                // For every entity, gets the Position and the RectangleSprite and draws it to the screen
                for (const auto &entity : view) {
                    const Position &pos = view.get<Position>(entity);
                    const SpriteComponent &sprite = view.get<SpriteComponent>(entity);

                    sprite.getSprite()->draw(pos.x, pos.y);
                }

                // Releases the semaphore after drawing is done
                xSemaphoreGive(game.getScreenLock());
                xSemaphoreGive(game.getSwapBufferSignal());
                //xSemaphoreGive(drawHitboxSignal);
            }

            // The Task is delayed until the set time between frames has passed (1/fps seconds)
            vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
        }
    }
}

void drawHitboxesTask(void *registryPointer) {
    //std::cout << "Thread[" << std::this_thread::get_id() << "] running task: " << pcTaskGetName(xTaskGetCurrentTaskHandle()) << std::endl;
    Game& game = Game::get();

    auto &regMutex = game.getRegistry();

    // The last time this task was waked by the scheduler
    auto lastWake = xTaskGetTickCount();

    while (true) {
        // Try taking the Draw Signal semaphore. The task should only draw if a draw signal is given by the other render tasks
        if(xSemaphoreTake(game.getDrawHitboxSignal(), portMAX_DELAY) == pdTRUE) {

            if(auto regOpt = regMutex.lock()) {
                Guard<entt::registry> &registry = *regOpt;

                // Get all entities with a Position and a Rectangle Sprite component
                auto view = registry->view<Position, Hitbox>();

                // Takes the screenLock Semaphore to have exclusive access to drawing
                xSemaphoreTake(game.getScreenLock(), portMAX_DELAY);
                // For every entity, gets the Position and the RectangleSprite and draws it to the screen
                for (const auto &entity : view) {
                    const Position &pos = view.get<Position>(entity);
                    const Hitbox &hitbox = view.get<Hitbox>(entity);

                    tumDrawBox(pos.x - 1, pos.y - 1, hitbox.width + 2, hitbox.height + 2, 0xFF0000);
                }
                // Releases the semaphore after drawing is done
                xSemaphoreGive(game.getScreenLock());
            }

            // The Task is delayed until the set time between frames has passed (1/fps seconds)
            vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
        }
    }
}

void movementTask(void *registryPointer) {
    //std::cout << "Thread[" << std::this_thread::get_id() << "] running task: " << pcTaskGetName(xTaskGetCurrentTaskHandle()) << std::endl;
    auto &regMutex = Game::get().getRegistry();
    auto lastWake = xTaskGetTickCount(); // Only used to delay the task by the correct time
    auto lastRun = lastWake; // Used to calculate the time difference between now and the last time the function was run

    while (true) {
        std::cout << "move" << std::endl;
        double dt = (xTaskGetTickCount() - lastRun) / 1000.0; // The time between now and the last run of this task in seconds


        if(auto regOpt = regMutex.lock()) {
            auto registry = *std::move(regOpt);

            registry->view<Position, Velocity>().each([dt](Position &pos, Velocity &vel) {
                pos.x += vel.dx * dt;
                pos.y += vel.dy * dt;
            });
        }

        lastRun = xTaskGetTickCount();
        vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
    }
}

void bounceTask(void *registryPointer) {
    //std::cout << "Thread[" << std::this_thread::get_id() << "] running task: " << pcTaskGetName(xTaskGetCurrentTaskHandle()) << std::endl;
    auto &regMutex = Game::get().getRegistry();
    auto lastWake = xTaskGetTickCount();

    while (true) {
        std::cout << "bounce" << std::endl;

        if (auto regOpt = regMutex.lock()) {
            auto registry = *std::move(regOpt);
            registry->view<Position, Velocity, Hitbox>().each([](Position &pos, Velocity &vel, Hitbox &hitbox) {
                if ((pos.x < 0 && vel.dx < 0) || (pos.x + hitbox.width > SCREEN_WIDTH && vel.dx > 0)) {
                    vel.dx = -vel.dx;
                }
                if ((pos.y < 0 && vel.dy < 0) || (pos.y + hitbox.height > SCREEN_HEIGHT && vel.dy > 0)) {
                    vel.dy = -vel.dy;
                }
            });
        }


        vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
    }
}

void spawnTask(void *_) {
    //std::cout << "Thread[" << std::this_thread::get_id() << "] running task: " << pcTaskGetName(xTaskGetCurrentTaskHandle()) << std::endl;
    auto &regMutex = Game::get().getRegistry();
    auto lastWake = xTaskGetTickCount(); // Only used to delay the task by the correct time

    static const short squareSize = 20;

    const Sprite &rectSprite = RectangleSprite{squareSize, squareSize, 0x00ff00, false};
    const Sprite &rectSpriteFilled = RectangleSprite{squareSize, squareSize, 0x00ff00, true};
    std::unique_ptr<Sprite> texSprite = std::make_unique<TextureSprite>("freertos.jpg");

    static auto i = 0;
    while (true) {
        std::cout << "spawn" << std::endl;

        if (auto regOpt = regMutex.lock()) {
            Guard<entt::registry> &regGuard = *regOpt;
            entt::registry &registry = *regGuard;

            // Creates a square with the corresponding components
            auto entity = registry.create();
            auto angle = 2 * M_PI * i / 80 + 1;

            registry.emplace<Position>(entity, SCREEN_WIDTH / 2 + 200 * std::sin(-angle),
                                       SCREEN_HEIGHT / 2 + 200 * std::cos(-angle));
            registry.emplace<Velocity>(entity, MOVE_SPEED * std::sin(angle), MOVE_SPEED * std::cos(angle));
            registry.emplace<SpriteComponent>(entity, (i % 2) ? &rectSprite : &rectSpriteFilled);
            registry.emplace<Hitbox>(entity, squareSize, squareSize);

            i++;
        }
        // The task should be delayed until 1/6th second has passed
        vTaskDelayUntil(&lastWake, FRAME_TIME_MS * TARGET_FPS / 2);
    }
}

TestState::TestState() {
    // Spawn the task that will render the rectangles
    TaskHandle_t render = nullptr;
    xTaskCreate(renderTask, "render", DEFAULT_STACK_SIZE, nullptr, 1, &render);
    tasks.push_back(render);

    // Spawn the task that will render the hitboxes
    TaskHandle_t drawHitboxes = nullptr;
    xTaskCreate(drawHitboxesTask, "hitboxes", DEFAULT_STACK_SIZE, nullptr, 1, &drawHitboxes);
    tasks.push_back(drawHitboxes);

    // Spawn the task that will move all entities with a Position and Velocity
    TaskHandle_t movement = nullptr;
    xTaskCreate(movementTask, "movement", 0, nullptr, 1, &movement);
    tasks.push_back(movement);

    // Spawn the task that will cause entities with a Position, Velocity, and hitbox to bounce off the edges of the screen
    TaskHandle_t bounce = nullptr;
    xTaskCreate(bounceTask, "bounce", DEFAULT_STACK_SIZE, nullptr, 1, &bounce);
    tasks.push_back(bounce);

    // Spawns the task that will spawn squares
    TaskHandle_t spawn = nullptr;
    xTaskCreate(spawnTask, "spawn", DEFAULT_STACK_SIZE, nullptr, 1, &spawn);
    tasks.push_back(spawn);
}
