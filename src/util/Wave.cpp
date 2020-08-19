//
// Created by hendrik on 16.07.20.
//

#include <algorithm>
#include "Wave.h"



// Constants used to determine the values of the next wave by scaling the values
// using these constants
inline const auto SPAWN_LIMIT_LINEAR_SCALING = 3.0;
inline const auto SPAWN_LIMIT_MULT_SCALING = 1.2;
inline const auto ENEMY_HEALTH_FACTOR_SCALING = 1.2;
inline const auto COIN_INCREASE_AFTER_ROUNDS = 5;


Wave::Wave(int spawnLimit, double enemyHealthFactor, int enemyCoins, int waveNumber): spawnLimit(spawnLimit), remainingSpawns(spawnLimit), enemyHealthFactor(enemyHealthFactor), enemyCoins(enemyCoins), remainingEnemies(remainingSpawns), waveNumber(waveNumber){}

int Wave::getWaveNumber() const {
    return waveNumber;
}

int Wave::getRemainingSpawns() const {
    return remainingSpawns;
}

double Wave::getEnemyHealthFactor() const {
    return enemyHealthFactor;
}

int Wave::getEnemyCoins() const {
    return enemyCoins;
}

void Wave::decrementRemainingSpawns() {
    remainingSpawns--;
}

int Wave::getRemainingEnemies() const {
    return remainingEnemies;
}

void Wave::setRemainingEnemies(int remainingEnemies) {
    Wave::remainingEnemies = remainingEnemies;
}

int Wave::getWaveNumber() {
    return waveNumber;
}

int Wave::getSpawnLimit() const {
    return spawnLimit;
}

bool Wave::isFinished() const {
    return remainingEnemies == 0;
}

void Wave::decrementRemainingEnemies() {
    remainingEnemies--;
}

Wave Wave::next() {
    return Wave(std::max(spawnLimit + SPAWN_LIMIT_LINEAR_SCALING, spawnLimit * SPAWN_LIMIT_MULT_SCALING),
            enemyHealthFactor * ENEMY_HEALTH_FACTOR_SCALING,
                1 + (waveNumber / COIN_INCREASE_AFTER_ROUNDS),
                waveNumber + 1);
}
