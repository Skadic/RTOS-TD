#pragma once


class Wave {
    int waveNumber;
    int enemyCount;
    int spawnLimit;
    double enemyHealthFactor;
    int enemyCoins;
    int remainingEnemies;
public:
    explicit Wave(int spawnLimit, double enemyHealthFactor, int enemyCoins, int waveNumber);

    bool isFinished() const;

    int getWaveNumber() const;

    int getEnemyCount() const;

    double getEnemyHealthFactor() const;

    int getEnemyCoins() const;

    int getWaveNumber();

    int getSpawnLimit() const;

    void decreaseEnemyCount();

    int getRemainingEnemies() const;

    void setRemainingEnemies(int remainingEnemies);

    void decrementRemainingEnemies();
};
