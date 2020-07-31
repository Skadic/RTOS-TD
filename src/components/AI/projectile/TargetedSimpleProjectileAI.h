
#include "../AI.h"

class TargetedSimpleProjectileAI : public AI {
    entt::entity target;
    double speed;
    unsigned int lifetime;
public:
    TargetedSimpleProjectileAI(entt::entity self, entt::entity target, double speed);
    void act(entt::registry &registry) override;
};
