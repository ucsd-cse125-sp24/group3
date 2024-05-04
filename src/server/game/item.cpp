#include "server/game/item.hpp"
#include "shared/game/sharedobject.hpp"

/*  Constructors and Destructors    */
Item::Item() : Object(ObjectType::Item) {

}

Item::~Item() {

}

/*	SharedGameState generation	*/
SharedObject Item::toShared() {
    auto so = Object::toShared();
    so.iteminfo = this->iteminfo;
    return so;
}