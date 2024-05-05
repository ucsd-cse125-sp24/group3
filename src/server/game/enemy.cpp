#include "server/game/enemy.hpp"
#include "shared/game/sharedobject.hpp"

SharedObject Enemy::toShared() {
    auto so = Creature::toShared();
    return so;
}

Enemy::Enemy() : Creature(ObjectType::Enemy) {
    this->stats = EnemyStats { 100 };
}

Enemy::~Enemy() {}