#pragma once

#include <entity/registry.hpp>
#include <TUM_Draw.h>

inline const unsigned int TARGET_FPS = 144;
inline const unsigned int FRAME_TIME_MS = 1000 / TARGET_FPS;

inline const auto DEFAULT_TASK_STACK_SIZE = 100;

inline const auto TILE_SIZE = 32;
inline const auto PLAYER_SIZE = TILE_SIZE / 2;

inline const auto PLAYER_SPEED = 1.5;
inline const auto ENEMY_SPEED = PLAYER_SPEED / 2.0;
inline const auto ENEMY_BASE_HEALTH = 100;

inline const auto DEFAULT_PROJECTILE_SIZE = PLAYER_SIZE / 4;
inline const auto DEFAULT_PROJECTILE_SPEED = 10;

inline const auto UI_BG_COLOR = 0x202020;
inline const auto UI_BORDER_COLOR = 0x909090;

inline const auto INGAME_BG_COLOR = 0x240500;
//inline const auto INGAME_BG_COLOR = 0x777777;

inline const auto MENU_BUTTON_WIDTH = SCREEN_WIDTH / 4;
inline const auto MENU_BUTTON_HEIGHT = MENU_BUTTON_WIDTH / 3;

inline const auto NEXUS_HEALTH = 20;

inline const auto LASER_TOWER_RANGE = 2 * TILE_SIZE;
inline const auto LASER_TOWER_DAMAGE = 2;

inline const auto PROJECTILE_TOWER_RANGE = 4 * TILE_SIZE;
inline const auto PROJECTILE_TOWER_DAMAGE = 20;
inline const auto PROJECTILE_FIRE_INTERVAL = 400;

inline const auto AOE_TOWER_RANGE = 3 * TILE_SIZE;
inline const auto AOE_TOWER_DAMAGE = 75;
inline const auto AOE_FIRE_INTERVAL = 750;

inline const auto INITIAL_COINS = 25;