#include "server/game/item.hpp"
#include "shared/game/sharedobject.hpp"

/*  Constructors and Destructors    */
Item::Item() : Object(ObjectType::Item) { // cppcheck-suppress uninitMemberVar

}

Item::~Item() {

}

/*	SharedGameState generation	*/
SharedObject Item::toShared() {
    auto so = Object::toShared();
    so.iteminfo = this->iteminfo;
    return so;
}