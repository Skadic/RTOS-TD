
#include "../AI.h"

class SimpleProjectileAI : public AI {
    entt::entity target;
    double speed;
    unsigned int lifetime;
public:
    SimpleProjectileAI(entt::entity self, entt::entity target, double speed);
    void act(entt::registry &registry) override;
};
