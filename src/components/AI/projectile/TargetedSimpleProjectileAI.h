
#include "../AI.h"

/**
 * A projectile AI, that receives a target and flies towards it
 * once it collides with its target, it deals damage and marks itself for deletion
 */
class TargetedSimpleProjectileAI : public AI {
    /**
     * This projectile's target
     */
    entt::entity target;

    /**
     * This projectile's speed
     */
    double speed;

    /**
     * This projectiles lifetime. Once it has lived for as many ticks as denoted by this variable
     * it will delete itself.
     */
    unsigned int lifetime;
public:
    /**
     * Create a new AI with the given target and projectile speed
     * @param self The projectile executing this AI
     * @param target This projectile's target
     * @param speed This projectile's speed
     */
    TargetedSimpleProjectileAI(entt::entity self, entt::entity target, double speed);
    void act(entt::registry &registry) override;
};
