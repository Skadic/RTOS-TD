#pragma once

#include <entity/registry.hpp>

inline const unsigned int TARGET_FPS = 144;
inline const unsigned int FRAME_TIME_MS = 1000 / TARGET_FPS;

inline const auto DEFAULT_TASK_STACK_SIZE = 100;

inline const auto TILE_SIZE = 32;
inline const auto PLAYER_SIZE = TILE_SIZE / 2;

inline const auto PLAYER_SPEED = 1.5;
inline const auto ENEMY_SPEED = PLAYER_SPEED / 2.0;

inline const auto DEFAULT_PROJECTILE_SIZE = PLAYER_SIZE / 2;
