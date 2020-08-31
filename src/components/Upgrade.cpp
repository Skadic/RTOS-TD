

#include "Upgrade.h"


void Upgrade::levelUp() {
    this->level++;
}

void Upgrade::scaleDamage(Damage &damage) {
    damage.value = (int) (damage.baseValue + level * factor * damage.baseValue);
}