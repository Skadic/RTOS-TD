//
// Created by hendrik on 16.07.20.
//

#ifndef RTOS_TD_WAVES_H
#define RTOS_TD_WAVES_H


class waves {
    int waveNumber;
    int enemyCount;
    int enemyHealth;
    int enemyCoins;
public:
    explicit waves(short enemyCount, short enemyHealth, float enemyCoins);
};


#endif //RTOS_TD_WAVES_H
