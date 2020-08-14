#pragma once

/**
 * A component representing the health of an entity as well as its max Health.
 */
struct Health {
    /**
     * The maximum health value of this entity
     */
    int maxHealth;

    /**
     * The current health value of this entity
     */
    int value;
};
