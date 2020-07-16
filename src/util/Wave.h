//
// Created by hendrik on 16.07.20.
//

#ifndef RTOS_TD_WAVE_H
#define RTOS_TD_WAVE_H


class Wave {
    int waveNumber;
    int enemyCount;
    int spawnLimit;
    double enemyHealthFactor;
    int enemyCoins;
    int remainingEnemies;
public:
    explicit Wave(int spawnLimit, double enemyHealthFactor, int enemyCoins, int waveNumber);

    int getWaveNumber() const;

    int getEnemyCount() const;

    double getEnemyHealthFactor() const;

    int getEnemyCoins() const;

    int getWaveNumber();

    int getSpawnLimit() const;

    void decreaseEnemyCount();

    int getRemainingEnemies() const;

    void setRemainingEnemies(int remainingEnemies);
};


#endif //RTOS_TD_WAVE_H
