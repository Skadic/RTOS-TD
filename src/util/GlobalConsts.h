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

inline const auto DEFAULT_PROJECTILE_SIZE = PLAYER_SIZE / 4;

inline const auto UI_BG_COLOR = 0x202020;
inline const auto UI_BORDER_COLOR = 0x909090;

inline const auto MENU_BUTTON_WIDTH = SCREEN_WIDTH / 4;
inline const auto MENU_BUTTON_HEIGHT = MENU_BUTTON_WIDTH / 2;

inline const auto NEXUS_HEALTH = 1;

