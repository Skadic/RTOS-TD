//
// Created by skadic on 21.05.20.
//

#include "iostream"
#include <entt.hpp>
#include <chrono>
#include <SDL2/SDL_scancode.h>
#include <cmath>
#include "components/Components.h"

extern "C" {
    #include "TUM_Draw.h"
    #include "TUM_Utils.h"
    #include "TUM_Event.h"
    #include "task.h"
    #include "semphr.h"
}

using namespace std;
using namespace chrono;

inline const unsigned int TARGET_FPS = 144;
inline const unsigned int FRAME_TIME_MS = 1000 / TARGET_FPS;

inline const float MOVE_SPEED = 300;

inline SemaphoreHandle_t DRAW_SIGNAL = nullptr;
inline SemaphoreHandle_t SCREEN_LOCK = nullptr;


unsigned int get_color(unsigned int i) {
    i = i % 0x600;
    if (i < 0x100) {
        return 0xFF0000 + ((i % 0x100) << 8); // Red to yellow
    } else if (i < 0x200) {
        return 0xFFFF00 - ((i % 0x100) << 16); // Yellow to green
    } else if (i < 0x300) {
        return (0x00FF00) + (i % 0x100); // Green to Cyan
    } else if (i < 0x400) {
        return (0x00FFFF) - ((i % 0x100) << 8); // Cyan to Blue
    } else if (i < 0x500) {
        return (0x0000FF) + ((i % 0x100) << 16); // Blue to Magenta
    } else if (i < 0x600) {
        return (0xFF00FF) - (i % 0x100); // Magenta to Red
    }
}

void swap_buffer_task(void *registryPointer) {
    auto lastWake = xTaskGetTickCount();

    // Get Draw access for this thread
    tumDrawBindThread();
    while (true) {
        // Take exclusive access to the screen
        if (xSemaphoreTake(SCREEN_LOCK, portMAX_DELAY) == pdTRUE) {
            cout << "swap buffer" << endl;

            // Update the Screen
            tumDrawUpdateScreen();

            // Update events (like keyboard and mouse input)
            tumEventFetchEvents();

            // Release the SCREEN_LOCK and give a draw signal
            xSemaphoreGive(SCREEN_LOCK);
            xSemaphoreGive(DRAW_SIGNAL);

            // Delay until the next frame
            vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
        }
    }
}

// The task that should render the Rectangles to the screen
void render_rect_task(void *registryPointer) {
    // Cast the void pointer that was supplied when spawning the task
    // The argument given there is just a pointer to the registry
    auto &registry = *static_cast<entt::registry*>(registryPointer);

    // The last time this task was waked by the scheduler
    auto lastWake = xTaskGetTickCount();

    while (true) {
        // If the Semaphore has been set (= if it's not a null pointer)
        if(DRAW_SIGNAL) {
            // Try taking the Draw Signal semaphore. The task should only draw if a draw signal is given by the swap_buffer task
            if(xSemaphoreTake(DRAW_SIGNAL, portMAX_DELAY) == pdTRUE) {
                // Get all entities with a Position and a Rectangle Sprite component
                auto view = registry.view<Position, RectangleSprite>();
                cout << "draw rect" << endl;

                // Takes the SCREEN_LOCK Semaphore to have exclusive access to drawing
                xSemaphoreTake(SCREEN_LOCK, portMAX_DELAY);
                // Clears the screen to black
                tumDrawClear(0x000000);

                // For every entity, gets the Position and the RectangleSprite and draws it to the screen
                for (auto entity : view) {
                    Position pos = view.get<Position>(entity);
                    RectangleSprite sprite = view.get<RectangleSprite>(entity);

                    sprite.draw(pos.x, pos.y);
                }

                // Releases the semaphore after drawing is done
                xSemaphoreGive(SCREEN_LOCK);

                // The Task is delayed until the set time between frames has passed (1/fps seconds)
                vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
            }
        }
    }
}

void movement_task(void *registryPointer) {
    auto &registry = *static_cast<entt::registry*>(registryPointer);
    auto lastWake = xTaskGetTickCount(); // Only used to delay the task by the correct time
    auto lastRun = lastWake; // Used to calculate the time difference between now and the last time the function was run

    while (true) {
        cout << "movement" << endl;
        double dt = (xTaskGetTickCount() - lastRun) / 1000.0; // The time between now and the last run of this task in seconds

        registry.view<Position, Velocity>().each([dt](Position &pos, Velocity &vel) {
            pos.x += vel.dx * dt;
            pos.y += vel.dy * dt;
        });

        lastRun = xTaskGetTickCount();
        vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
    }
}

void bounce_task(void *registryPointer) {
    auto &registry = *static_cast<entt::registry*>(registryPointer);
    auto lastWake = xTaskGetTickCount();

    while (true) {
        cout << "bounce" << endl;
        registry.view<Position, Velocity, Hitbox>().each([](Position &pos, Velocity &vel, Hitbox &hitbox) {
            if (pos.x < 0 || pos.x + hitbox.width > SCREEN_WIDTH) {
                 vel.dx = -vel.dx;
             } else if (pos.y < 0 || pos.y + hitbox.height > SCREEN_HEIGHT) {
                 vel.dy = -vel.dy;
             }
        });

        vTaskDelayUntil(&lastWake, FRAME_TIME_MS);
    }
}

void spawn_task(void *registryPointer) {
    auto &registry = *static_cast<entt::registry*>(registryPointer);
    auto lastWake = xTaskGetTickCount(); // Only used to delay the task by the correct time

    static auto i = 0;
    static const short square_size = 20;

    while (true) {
        cout << "spawn" << endl;

        // Creates a square with the corresponding components
        auto entity = registry.create();
        auto angle = 2 * 3.1415 * i / 10 + 1;
        registry.emplace<Position>(entity, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
        registry.emplace<Velocity>(entity, MOVE_SPEED * sin(angle), MOVE_SPEED * cos(angle));
        registry.emplace<RectangleSprite>(entity, square_size, square_size, get_color(i * 20), i % 2);
        registry.emplace<Hitbox>(entity, square_size, square_size);

        i++;

        // The task should be delayed until 1/6th second has passed
        vTaskDelayUntil(&lastWake, FRAME_TIME_MS * TARGET_FPS / 6);
    }
}

int main(int argc, char* argv[]) {

    // The registry containing all entities of our game
    entt::registry registry;

    // Get the path of the folder which contains the binary running the program
    auto path = tumUtilGetBinFolderPath(argv[0]);

    // Init event handling and drawing
    tumEventInit();
    tumDrawInit(path);

    // Create semaphores to synchronize access to the draw commands and for the swap_buffer task to give a draw signal
    DRAW_SIGNAL = xSemaphoreCreateBinary();
    SCREEN_LOCK = xSemaphoreCreateMutex();


    // Spawn the task that will update the screen each frame
    TaskHandle_t swap_buffer = nullptr;
    xTaskCreate(swap_buffer_task, "swap_buffer", 200, &registry, 2, &swap_buffer);

    // Spawn the task that will render the rectangles
    TaskHandle_t rect_task = nullptr;
    xTaskCreate(render_rect_task, "render_rect", 200, &registry, 1, &rect_task);

    // Spawn the task that will move all entities with a Position and Velocity
    TaskHandle_t movement = nullptr;
    xTaskCreate(movement_task, "movement", 200, &registry, 1, &movement);

    // Spawn the task that will cause entities with a Position, Velocity, and hitbox to bounce off the edges of the screen
    TaskHandle_t bounce = nullptr;
    xTaskCreate(bounce_task, "bounce", 200, &registry, 1, &bounce);

    // Spawns the task that will spawn squares
    TaskHandle_t spawn = nullptr;
    xTaskCreate(spawn_task, "spawn", 200, &registry, 1, &spawn);

    // Starts the task scheduler to start running the tasks
    vTaskStartScheduler();


    // Wait for input from the console (so that the program doesn't exit immediately)
    getchar();

    // Delete all tasks
    vTaskDelete(rect_task);
    vTaskDelete(swap_buffer);
    vTaskDelete(movement);
    vTaskDelete(bounce);
    vTaskDelete(spawn);
}


