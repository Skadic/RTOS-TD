//
// Created by hendrik on 16.07.20.
//

#include "Wave.h"


Wave::Wave(int spawnLimit, double enemyHealth, int enemyCoins, int waveNumber): spawnLimit(spawnLimit),enemyCount(spawnLimit), enemyHealthFactor(enemyHealth), enemyCoins(enemyCoins), remainingEnemies(enemyCount), waveNumber(waveNumber){}

int Wave::getWaveNumber() const {
    return waveNumber;
}

int Wave::getEnemyCount() const {
    return enemyCount;
}

double Wave::getEnemyHealthFactor() const {
    return enemyHealthFactor;
}

int Wave::getEnemyCoins() const {
    return enemyCoins;
}

void Wave::decreaseEnemyCount() {
    enemyCount--;
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
