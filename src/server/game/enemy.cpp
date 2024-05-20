#include "server/game/enemy.hpp"
#include "shared/game/sharedobject.hpp"

SharedObject Enemy::toShared() {
    auto so = Creature::toShared();
    return so;
}

Enemy::Enemy(glm::vec3 corner, glm::vec3 facing, ObjectType type, ModelType model, SharedStats&& stats):
    Creature(type, corner, facing, model, std::move(stats))
{}

Enemy::~Enemy() {}