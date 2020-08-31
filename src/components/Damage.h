#pragma once

/**
 * A component representing the damage of an entity
 */
struct Damage {
    int baseValue;
    int value;

    Damage(int value) : baseValue{value}, value{value} {}
};
