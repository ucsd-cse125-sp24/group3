#include "server/game/player.hpp"
#include "shared/game/sharedobject.hpp"
#include "shared/game/stat.hpp"
#include "server/game/item.hpp"
#include "server/game/potion.hpp"
#include <iostream>

SharedObject Player::toShared() {
    auto so = Creature::toShared();
    return so;
}

Player::Player():
    Creature(ObjectType::Player, SharedStats(
        Stat(0, 100, 50),
        Stat(0, 10, 5)
    ))
{
}

Player::~Player() {

}

void Player::useItem(int itemNum) {
    Item* item = this->inventory.at(itemNum);
    switch (item->iteminfo.type) {
    case ItemType::Potion: {
        Potion* pot = (Potion*)item;
        switch (pot->potType) {
        case PotionType::Health: {
            this->stats.health.adjustMod(pot->effectScalar);
            break;
        }
        case PotionType::Swiftness: {
            this->stats.speed.adjustMod(pot->effectScalar);
            break;
        }
        case PotionType::Invisibility: {

            break;
        }
        }
    }
    }

    this->inventory.erase(itemNum);
}