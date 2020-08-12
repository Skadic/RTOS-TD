#pragma once


class Wave {
    int waveNumber;
    int remainingSpawns;
    int spawnLimit;
    double enemyHealthFactor;
    int enemyCoins;
    int remainingEnemies;
public:
    explicit Wave(int spawnLimit, double enemyHealthFactor, int enemyCoins, int waveNumber);

    bool isFinished() const;
    int getWaveNumber() const;
    int getRemainingSpawns() const;
    double getEnemyHealthFactor() const;
    int getEnemyCoins() const;
    int getWaveNumber();
    int getSpawnLimit() const;
    int getRemainingEnemies() const;

    void decreaseRemainingSpawns();
    void setRemainingEnemies(int remainingEnemies);
    void decrementRemainingEnemies();

    Wave next();
};
