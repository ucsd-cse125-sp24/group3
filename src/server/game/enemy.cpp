#include "server/game/enemy.hpp"
#include "shared/game/sharedobject.hpp"

SharedObject Enemy::toShared() {
    auto so = Creature::toShared();
    return so;
}

Enemy::Enemy():
    Creature(ObjectType::Enemy, SharedStats(
        Stat(0, 20, 20),
        Stat(0, 10, 5)
    ))
{}

Enemy::~Enemy() {}