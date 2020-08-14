#pragma once

/**
 * Contains information about the current wave
 */
class Wave {
    /**
     * The wave number
     */
    int waveNumber;

    /**
     * How many enemies this wave is still allowed to spawn
     */
    int remainingSpawns;

    /**
     * The maximum spawn limit of this wave
     */
    int spawnLimit;

    /**
     * The multiplier with which the enemy's base health (@see GlobalConsts) is scaled with.
     */
    double enemyHealthFactor;

    /**
     * The amount of coins the enemies drop
     */
    int enemyCoins;

    /**
     * The amount of enemies that still remain to be defeated
     */
    int remainingEnemies;

public:
    /**
     * Creates a wave with the specified data
     */
    explicit Wave(int spawnLimit, double enemyHealthFactor, int enemyCoins, int waveNumber);

    /**
     * Returns whether the wave is finished (i.e. if there are no more enemies to defeat)
     * @return true, if the wave is finished, false otherwise.
     */
    bool isFinished() const;
    int getWaveNumber() const;
    int getRemainingSpawns() const;
    double getEnemyHealthFactor() const;
    int getEnemyCoins() const;
    int getWaveNumber();
    int getSpawnLimit() const;
    int getRemainingEnemies() const;

    void decrementRemainingSpawns();
    void setRemainingEnemies(int remainingEnemies);
    void decrementRemainingEnemies();

    /**
     * Returns a wave with values scaled according to the constants found in Wave.cpp
     * @return A new wave with scaled values
     */
    Wave next();
};
