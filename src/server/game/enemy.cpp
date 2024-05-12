#include "server/game/enemy.hpp"
#include "shared/game/sharedobject.hpp"

SharedObject Enemy::toShared() {
    auto so = Creature::toShared();
    return so;
}

Enemy::Enemy(glm::vec3 corner, glm::vec3 facing):
    Creature(ObjectType::Enemy, corner, facing, ModelType::WarrenBear, SharedStats(
        Stat(0, 100, 100),
        Stat(0, 10, 5)
    ))
{}

Enemy::~Enemy() {}